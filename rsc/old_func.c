/* @brief display board in stdout Unused */
// void display_uint16_array(u32 *array)
// {
// 	ft_printf_fd(1, "\n\nDisplay board\n");
// 	u32 i = 0, j = 0;
// 	(void)array;

// 	for (i = 0; i < BOARD_H; i++) {
// 		for (j = 0; j < BOARD_W; j++) {
// 			u32 idx = i * BOARD_W + j;
// 			// ft_printf_fd(2, "For idx = %u, vec = [%u][%u]\n", idx, get_board_vec(idx).y, get_board_vec(idx).x);
// 			ft_printf_fd(1, YELLOW"["RESET""CYAN"%u"RESET""YELLOW"] "RESET, array[idx]);
// 		}
// 		ft_printf_fd(1, "\n");
// 	}
// 	ft_printf_fd(1, "\n");
// }

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
