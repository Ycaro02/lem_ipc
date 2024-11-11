# include "../include/lem_ipc.h"

/**
 *	@brief Get message queue id
 *	@param key The key reference the message queue
 *	@param flag The flag to create or not the message queue
 *	@return The message queue id, -1 on error 
*/
int get_msg_queue(key_t key, int flag)
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
 *	@brief Clear the message queue
 *	@param ipc The ipc structure
 *	@param team_id The channel id to clear
 *	@return 0 on success, -1 on error
*/
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
 *	@brief Fill the message buffer
 *	@param msg The message buffer
 *	@param team_id The team id
 *	@param data The data to send
*/
static void fill_msgbuff(t_msgbuf *msg, uint32_t team_id, uint32_t data)
{
	char *ptr = (char *)&data;

	msg->mtype = team_id;
	msg->mtext[0] = ptr[0];
	msg->mtext[1] = ptr[1];
	msg->mtext[2] = ptr[2];
	msg->mtext[3] = ptr[3];
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
	// ft_printf_fd(1, YELLOW"Sending message to team %u value: %u\n"RESET, msg_id, data);
	// ft_printf_fd(1, YELLOW"msg.text [%d|%d|%d|%d]\nAfter cast: [%u] \n"RESET, msg.mtext[0], msg.mtext[1], msg.mtext[2], msg.mtext[3], (*(uint32_t *)msg.mtext));
	errno = 0;
	if (msgsnd(ipc->msgid, &msg, sizeof(uint32_t), IPC_NOWAIT) == -1) {
		 if (errno == EAGAIN) {
			// ft_printf_fd(2, RED"Message queue is full Clean it\n"RESET);
			clear_msg_queue(ipc, 0);
			return (0);
		}
		// ft_printf_fd(2, RED"Error msgsend from %d\n", msg_id, RESET);
		// syscall_perror("msgsnd");
		return (-1);
	}
	return (0);
}


/**
 *	@brief Extract a message from the message queue
 *	@param ipc The ipc structure
 *	@param msg_id The message id
 *	@return The message value, UINT32_MAX on error
*/
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

