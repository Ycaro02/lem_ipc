# include "../include/lem_ipc.h"

void display_msg_queue_size(int msgid)
{
	struct msqid_ds buf = {};

	errno = 0;
	if (msgctl(msgid, IPC_STAT, &buf) == -1) {
		syscall_perror("msgctl");
		return ;
	}
	// display message queue nb msg
	ft_printf_fd(1, YELLOW"Current Number of message in queue: %u -> "RESET, buf.msg_qnum);
	// display message queue size in bytes
	ft_printf_fd(1, YELLOW"size in bytes: %u\n"RESET, buf.__msg_cbytes);
	// display message queue max size in bytes
	// ft_printf_fd(1, YELLOW"Message queue max size in bytes: %u\n"RESET, buf.msg_qbytes);
}

/**
 *	@brief Get the current message queue size
 *	@param msgid The message queue id
 *	@return The message queue size, 0 on error
*/
u32 message_queue_size_get(int msgid)
{
	struct msqid_ds buf = {};

	errno = 0;
	if (msgctl(msgid, IPC_STAT, &buf) == -1) {
		syscall_perror("msgctl");
		return (0);
	}
	return (buf.__msg_cbytes);
}

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
s8 remove_msg_queue(IPC *ipc)
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
 *	@param chan_id The channel id to clear
 *	@return 0 on success, -1 on error
*/
s8 clear_msg_queue(IPC *ipc, long chan_id)
{
	MsgBuf msg = {};
	s8 ret = 0;
	ssize_t msg_ret = 0;

	errno = 0;
	ft_printf_fd(1, GREEN"Clearing message queue CHAN %u\n"RESET, chan_id);
	display_msg_queue_size(ipc->msgid);
	// while (msgrcv(ipc->msgid, &msg, sizeof(u32), 0, IPC_NOWAIT) != -1) {
	while (1) {
		msg_ret = msgrcv(ipc->msgid, &msg, sizeof(u32), chan_id, IPC_NOWAIT);
		if (msg_ret == -1 && errno == ENOMSG) {
			ret = -1;
			break ;
		}
		errno = 0;
	}
	ft_printf_fd(1, CYAN"Message queue clear: "RESET, chan_id);
	display_msg_queue_size(ipc->msgid);
	return (ret);

}

/**
 *	@brief Fill the message buffer
 *	@param msg The message buffer
 *	@param team_id The team id
 *	@param data The data to send
*/
static void fill_msgbuff(MsgBuf *msg, u32 team_id, u32 data)
{
	char *ptr = (char *)&data;

	msg->mtype = team_id;
	msg->mtext[0] = ptr[0];
	msg->mtext[1] = ptr[1];
	msg->mtext[2] = ptr[2];
	msg->mtext[3] = ptr[3];
}


/**
 * @brief Send a message to the message queue
 * @param ipc The ipc structure
 * @param msg_id The message id (channel id)
 * @param data The data to send
 * @param from_id The team id from the sender
 * @return 1 on success, -1 on error
 */
s8 send_msg(IPC *ipc, u32 msg_id, u32 data, u32 from_id)
{
	MsgBuf msg = {};

	(void)from_id;

	fill_msgbuff(&msg, msg_id, data);
	errno = 0;
	if (msgsnd(ipc->msgid, &msg, sizeof(u32), IPC_NOWAIT) == -1) {
		//  if (errno == EAGAIN) { /* message queue full */
		 if (errno == EAGAIN) { /* message queue full */
			// ft_printf_fd(2, RED"Message queue is full Clean it\n"RESET);
			if (!ipc->display) {
				clear_msg_queue(ipc, 0);
			}
			return (FALSE);
		}
		ft_printf_fd(2, RED"Error msgsnd, errno %d\n"RESET, errno);
		return (-1);
	}
	return (TRUE);
}


/**
 *	@brief Extract a message from the message queue
 *	@param ipc The ipc structure
 *	@param msg_id The message id
 *	@return The message value, UINT32_MAX on error
*/
u32 extract_msg(IPC *ipc, u32 msg_id)
{
	MsgBuf msg = {};
	// int cpy_flag = 040000;
	errno = 0;
	// ft_printf_fd(1, GREEN"Extracting message from team %d, val flag %d\n"RESET, msg_id, IPC_NOWAIT);
	if (msgrcv(ipc->msgid, &msg, sizeof(u32), msg_id, IPC_NOWAIT) == -1) {
		if (errno == ENOMSG) {
			// ft_printf_fd(2, YELLOW"No msg rcv from %d\n", msg_id, RESET);
			return (UINT32_MAX);
		}
		ft_printf_fd(2, RED"Error msgrcv from %d\n", msg_id, RESET);
		syscall_perror("msgrcv");
		return (UINT32_MAX);
	}
	// ft_printf_fd(1, PURPLE"Received message from team %d value: %u\n"RESET, msg_id, (*(u32 *)msg.mtext));
	return (*(u32 *)msg.mtext);
}

