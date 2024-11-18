# include "../include/lem_ipc.h"

/**
 * @brief Get key from file path
 * @param path The file path
*/
static int file_to_key(char *path)
{
	key_t key;

	errno = 0;
	key = ftok(path, IPC_PROJ_ID);
	if (key == (key_t)-1) {
		syscall_perror("ftok");
		return (-1);
	}
	return (key);
}

/**
 * @brief Get semaphore set id
 * @param key The key reference the semaphore
*/
static int get_sem_set_id(key_t key) 
{
	int shmid;

	errno = 0;
	shmid = semget(key, 1, 0666);
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	// ft_printf_fd(1, GREEN"Child get shared mem ID  %d\n"RESET, SHM_DATA_SIZE, ALIGN_SHARED_MEM);
	return (shmid);
}


#define		RESET_LINE			"\r\033[K"
/**
 * @brief Attach shared memory
 * @param ipc The ipc structure
 * @return 0 for server case (first launch), 1 for a child (basic client or display handler), -1 on error
*/
static int shared_rsc_handler(IPC *ipc, s8 allow)
{
	int flag = 0666;
	
	if (allow)
		flag |= (IPC_CREAT | IPC_EXCL);
	errno = 0;
	ipc->semid = semget(ipc->key, 1, (flag | 0666));
	if (allow == 0 && ipc->semid == -1) { /* if error and can't create sem (vizualizer case) */
		if (allow) {
			syscall_perror("semget");
		} else {
			ft_printf_fd(2, RESET_LINE""YELLOW"Displayer waiting for IPC init ... "RESET);
		}
		return (ERROR_CASE);
	} else if ((!allow && ipc->semid != -1 ) || (allow && ipc->semid == -1)) { /* if ressource already created */
		ipc->semid = get_sem_set_id(ipc->key);
		ipc->shmid = get_shared_memory(ipc->key, 0);
		ipc->msgid = get_msg_queue(ipc->key, 0);
		if (ipc->shmid == -1 || ipc->semid == -1 || ipc->msgid == -1) {
			ft_printf_fd(2, RED"Error child can't get shared data"RESET);
			return (ERROR_CASE); /* error case */
		} 
		sem_lock(ipc->semid);
		if (attach_shared_memory(ipc) == -1) {
			return (ERROR_CASE);
		}
		sem_unlock(ipc->semid);
		return (CLIENT_CASE); /* child case */
	} 
	return (SERVER_CASE); /* parent case */
}

/**
 *	@brief check if file exist and have good perm
 *	@param path The file path
 *	@return 1 on success, 0 on error
*/
static s8 chek_path_exist(char *path)
{
	int fd = -1;
	if (access(path, F_OK | R_OK | W_OK) == -1) {
		ft_printf_fd(2, YELLOW"Error file %s not found or bad perm, try to create it\n"RESET, path);
		errno = 0;
		fd = open(path, O_CREAT | O_RDWR, 0666);
		if (fd == -1) {
			syscall_perror("open");
			return (0);
		}
		ft_printf_fd(1, YELLOW"File %s created\n"RESET, path);
		return (1);
	}
	return (1);
}

/**
 * @brief Init shared memory
 * @param ipc The ipc structure
 * @return The shared memory id, -1 on error
*/
int iniGame(IPC *ipc, char *path, s8 allow)
{
	/* Same here active protection when debug start is finish */

	if (!chek_path_exist(path)) {
		return (-1);
	}
	ipc->key = file_to_key(path);
	if (ipc->key == -1) {
		ft_printf_fd(2, RED"Error can't get key for %s"RESET, path);
		return (-1);
	}

	int ret = shared_rsc_handler(ipc, allow);
	if (ret != SERVER_CASE) { /* if not parent */
		return (ret);
	}

	/* If we are here we are the SERVER (first client )*/

	/* Set semaphore value to 0 (lock it) */
	if (semctl(ipc->semid, 0, SETVAL, 0)) {
		syscall_perror("semctl");
		return (ERROR_CASE);
	}
	
	ipc->shmid = get_shared_memory(ipc->key, IPC_CREAT | IPC_EXCL);
	ipc->msgid = get_msg_queue(ipc->key, IPC_CREAT | IPC_EXCL);
	if (ipc->shmid == -1 || ipc->msgid == -1 || attach_shared_memory(ipc) == -1) {
		return (ERROR_CASE);
	}
	struct msqid_ds buf = {};

	/* Get current msgqueue stats in buf */
	if (msgctl(ipc->msgid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT");
        exit(1);
    }

	/* Set the max number of bytes allowed in the queue */
    buf.msg_qbytes = MSG_QUEUE_SIZE;

	/* Set the new msgqueue stats */
    if (msgctl(ipc->msgid, IPC_SET, &buf) == -1) {
        perror("msgctl IPC_SET");
        exit(1);
    }

	set_playing_state(ipc->ptr, FALSE);

	// ft_printf_fd(1, PURPLE"Server started waiting client 8sec ...\n"RESET);
	sleep(1); /* wait for client to connect */
	ft_printf_fd(1, CYAN"Server send controle display packet\n"RESET);
	// here we send 0 to from_id cause it's the server, and player id are not define yet
	send_display_controle_packet(ipc, CTRL_DH_WAITING_TO_CONNECT, 0);


	sem_unlock(ipc->semid); /* put sem value to 1 to let other program connect */
	return (0);
}
