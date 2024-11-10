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

/**
 * @brief Attach shared memory
 * @param ipc The ipc structure
 * @return 0 for server case (first launch), 1 for a child (basic client or display handler), -1 on error
*/
static int shared_rsc_handler(t_ipc *ipc, int8_t allow)
{
	int flag = 0666;
	
	if (allow)
		flag |= (IPC_CREAT | IPC_EXCL);
	errno = 0;
	ipc->semid = semget(ipc->key, 1, (flag | 0666));
	if (allow == 0 && ipc->semid == -1) { /* if error and can't create sem (vizualizer case) */
		syscall_perror("semget");
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
static int8_t chek_path_exist(char *path)
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
int init_game(t_ipc *ipc, char *path, int8_t allow)
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
	/* Set semaphore value to 0 (lock it) */
	if (semctl(ipc->semid, 0, SETVAL, 0)) {
		syscall_perror("semctl");
		return (-1);
	}
	
	ipc->shmid = get_shared_memory(ipc->key, IPC_CREAT | IPC_EXCL);
	ipc->msgid = get_msg_queue(ipc->key, IPC_CREAT | IPC_EXCL);
	if (ipc->shmid == -1 || ipc->msgid == -1 || attach_shared_memory(ipc) == -1) {
		return (-1);
	}
	struct msqid_ds buf = {};

	if (msgctl(ipc->msgid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT");
        exit(1);
    }
    buf.msg_qbytes = MSG_QUEUE_SIZE;
    // buf.msg_qbytes = 16384;
    if (msgctl(ipc->msgid, IPC_SET, &buf) == -1) {
        perror("msgctl IPC_SET");
        exit(1);
    }


	// display msg_len
	// ft_printf_fd(1, YELLOW"Msg len: "RESET""CYAN"%u\n"RESET, get_msg_len(ipc));
	// ft_printf_fd(1, YELLOW"Uint32 max: "RESET""CYAN"%u\n"RESET, UINT32_MAX);
	// set_msg_len(ipc, sizeof(uint32_t));
	// ft_printf_fd(1, YELLOW"Semaphore value Before first send: "RESET""CYAN"%d\n"RESET, semctl(ipc->semid, 0, GETVAL));
	// send_msg(ipc, &(t_player){.team_id = 1}, 42);
	// send_msg(ipc, &(t_player){.team_id = 2}, 9);
	ft_printf_fd(1, PURPLE"Server started waiting client 5sec ...\n"RESET);
	sleep(5); /* wait for client to connect */
	ft_printf_fd(1, CYAN"Server send controle display packet\n"RESET);
	send_display_controle_packet(ipc);




	sem_unlock(ipc->semid); /* put sem value to 1 to let other program conext to mem */
	return (0);
}
