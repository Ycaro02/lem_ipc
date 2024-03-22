# include "../include/lem_ipc.h"


/**
 *	@brief Get message queue id
 *	@param key The key reference the message queue
 *	@param flag The flag to create or not the message queue
 *	@return The message queue id, -1 on error 
*/
static int get_msg_queue(key_t key, int flag)
{
	int msgid;

	errno = 0;
	msgid = msgget(key, (flag | 0666));
	if (msgid == -1) {
		syscall_perror("msgget");
		return (-1);
	}
	return (msgid);
}


/**
 *	@brief Remove a message queue
 *	@param ipc The ipc structure
 *	@return 0 on success, -1 on error
*/
int8_t remove_msg_queue(t_ipc *ipc)
{

	errno = 0;
	if (msgctl(ipc->msgid, IPC_RMID, NULL) == -1) {
		syscall_perror("msgctl");
		return (-1);
	}
	return (0);
}


/**
 *	@brief Set message length
 *	@param ipc The ipc structure
 *	@param len The new message length
 *	@return 0 on success, -1 on error
*/
int set_msg_len (t_ipc *ipc, msglen_t len)
{
	struct msqid_ds buf;

	errno = 0;
	if (msgctl(ipc->msgid, IPC_STAT, &buf) == -1) {
		syscall_perror("msgctl");
		return (-1);
	}
	buf.msg_qbytes = len;
	errno = 0;
	if (msgctl(ipc->msgid, IPC_SET, &buf) == -1) {
		syscall_perror("msgctl");
		return (-1);
	}
	return (0);
}


void fill_msgbuff(t_msgbuf *msg, uint32_t team_id, uint32_t data)
{
	char *ptr = (char *)&data;

	msg->mtype = team_id;
	msg->mtext[0] = ptr[0];
	msg->mtext[1] = ptr[1];
	msg->mtext[2] = ptr[2];
	msg->mtext[3] = ptr[3];
}

int8_t clear_msg_queue(t_ipc *ipc, long team_id)
{
	t_msgbuf msg = {};

	errno = 0;
	// ft_printf_fd(1, YELLOW"Clearing message queue from team %u\n"RESET, team_id);
	while (msgrcv(ipc->msgid, &msg, sizeof(uint32_t), team_id, IPC_NOWAIT) != -1) {
		if (errno != ENOMSG) {
			// syscall_perror("msgrcv");
			return (-1);
		}
	}
	return (0);

}

/**
 *	@brief Send a message to the message queue
 *	@param ipc The ipc structure
 *	@param player The player structure
 *	@return 0 on success, -1 on error
*/
int8_t send_msg(t_ipc *ipc, uint32_t msg_id, uint32_t data)
{
	t_msgbuf msg = {};

	fill_msgbuff(&msg, msg_id, data);
	// ft_printf_fd(1, YELLOW"Sending message to team %d value: %u\n"RESET, msg_id, data);
	// ft_printf_fd(1, YELLOW"msg.text [%d|%d|%d|%d]\nAfter cast: [%u] \n"RESET, msg.mtext[0], msg.mtext[1], msg.mtext[2], msg.mtext[3], (*(uint32_t *)msg.mtext));
	errno = 0;
	if (msgsnd(ipc->msgid, &msg, sizeof(uint32_t), 0) == -1) {
		 if (errno == EAGAIN) {
			ft_printf_fd(2, RED"Message queue is full\n"RESET);
			clear_msg_queue(ipc, 0);
			return (0);
		}
		ft_printf_fd(2, RED"Error msgsend from %d\n", msg_id, RESET);
		syscall_perror("msgsnd");
		return (-1);
	}
	return (0);
}

uint32_t extract_msg(t_ipc *ipc, uint32_t msg_id)
{
	t_msgbuf msg = {};
	// int cpy_flag = 040000;
	errno = 0;
	// ft_printf_fd(1, GREEN"Extracting message from team %d, val flag %d\n"RESET, msg_id, IPC_NOWAIT);
	if (msgrcv(ipc->msgid, &msg, sizeof(uint32_t), msg_id, IPC_NOWAIT) == -1) {
		if (errno == ENOMSG) {
			// ft_printf_fd(2, YELLOW"No msg rcv from %d\n", msg_id, RESET);
			return (UINT32_MAX);
		}
		ft_printf_fd(2, RED"Error msgrcv from %d\n", msg_id, RESET);
		syscall_perror("msgrcv");
		return (UINT32_MAX);
	}
	// ft_printf_fd(1, PURPLE"Received message from team %d value: %u\n"RESET, msg_id, (*(uint32_t *)msg.mtext));
	return (*(uint32_t *)msg.mtext);
}


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

	// display msg_len
	// ft_printf_fd(1, YELLOW"Msg len: "RESET""CYAN"%u\n"RESET, get_msg_len(ipc));
	// ft_printf_fd(1, YELLOW"Uint32 max: "RESET""CYAN"%u\n"RESET, UINT32_MAX);
	// set_msg_len(ipc, sizeof(uint32_t));
	// ft_printf_fd(1, YELLOW"Semaphore value Before first send: "RESET""CYAN"%d\n"RESET, semctl(ipc->semid, 0, GETVAL));
	// send_msg(ipc, &(t_player){.team_id = 1}, 42);
	// send_msg(ipc, &(t_player){.team_id = 2}, 9);

	sleep(1); /* wait for client to connect */

	sem_unlock(ipc->semid); /* put sem value to 1 to let other program conext to mem */
	return (0);
}
