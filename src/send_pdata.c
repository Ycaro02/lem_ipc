# include "../include/lem_ipc.h"


/**
 * @brief Display packet content, debug purpose
 * @ param data Packet data
*/
void display_packet(u32 *data)
{
	ft_printf_fd(1, CYAN"\n-------------------------Send packet --------------------------------------\n"RESET);
	ft_printf_fd(1, GREEN"start: |%u| "RESET, data[PDATA_START]);
	ft_printf_fd(1, GREEN"state/type: |%u| "RESET, data[PDATA_STATE]);
	ft_printf_fd(1, GREEN"tid: |%u| "RESET, data[PDATA_TID]);
	ft_printf_fd(1, GREEN"pos: |%u| "RESET, data[PDATA_POS]);
	ft_printf_fd(1, GREEN"target: |%u| "RESET, data[PDATA_TARGET]);
	ft_printf_fd(1, GREEN"ally: |%u|"RESET, data[PDATA_ALLY]);
	ft_printf_fd(1, GREEN"supp: |%u|\n"RESET, data[PDATA_SUPP]);
	ft_printf_fd(1, YELLOW"Type: |%u| state:|%u|"RESET, GET_MSG_TYPE(data[PDATA_STATE]), GET_MSG_STATE(data[PDATA_STATE]));
	ft_printf_fd(1, CYAN"\n-------------------------Packet end---------------------------------------\n"RESET);

}

/**
 * @brief Send display controle packet to display handler
 * @ param ipc IPC structure
 * @ param displayer_state Display handler state
 * @ param from_id Sender id
*/
void send_display_controle_packet(IPC *ipc, u32 displayer_state, u32 from_id)
{
	u32	data[PDATA_LEN] = BUILD_CTRL_PACKET(displayer_state);
	s8		ret = TRUE;

	for (int i = 0; i < PDATA_LEN; ++i) {
		ret = send_msg(ipc, CONTROLE_DISPLAY_CHAN, data[i], from_id);
		if (ret == FALSE) {
			break ;
		}
	}
	/* We do this cause send_msg clear the queue when return FALSE */ 
	if (ret == FALSE) {
		send_display_controle_packet(ipc, displayer_state, from_id);
	}
}

/**
 * @brief Check if display handler is connected
 * @ param ipc IPC structure
 * @ return DH_DISCONNECTED if display handler is not connected
 * @ return DH_CONNECTED if display handler is connected
 * @ return DH_PRIORITY if display handler is in priority mode
*/
s8 display_handler_state(IPC *ipc)
{
	u32	data[PDATA_LEN] = BUILD_CTRL_PACKET((u32)UINT32_MAX);
	int			i;
	s8		ret = DH_DISCONNECTED;

	for (i = 0; i < PDATA_LEN; ++i) {
		data[i] = extract_msg(ipc, CONTROLE_DISPLAY_CHAN);
		if (data[i] != CTRL_DH_WAITING_TO_CONNECT && data[i] != CTRL_DH_PRIORITY) {
			break;
		}
	}
	/* If we not found any data for displayer packet he is here and just normaly waiting */
	if (i == 0 && data[i] == UINT32_MAX) {
		ret = DH_CONNECTED;
	} 
	else if (i == PDATA_LEN) {
		/* Here we found a controle packet, if is a priority packet we need to update ret */
		if (data[0] == CTRL_DH_PRIORITY) {
			ret = DH_PRIORITY;
		}
		// ft_printf_fd(2, CYAN"Display handler packet found resend it with |%u|\n"RESET, data[0]);
		send_display_controle_packet(ipc, data[0], 0);
	}
	return (ret);
}

/**
 * @brief Wait for display handler to connect before starting game
 * @ param ipc IPC structure
*/
void wait_for_display_handler_connect(IPC *ipc) 
{
	while (display_handler_state(ipc) == DH_DISCONNECTED) {
		sem_unlock(ipc->semid);
		usleep(100000);
		sem_lock(ipc->semid);
	}
	ipc->display = display_handler_state(ipc);
}

/**
 * @brief Wait for display handler to process the queue
 * @ param ipc IPC structure
*/
void wait_for_display_handler_priority(IPC *ipc)
{
	u32 queue_size = MSG_QUEUE_LIMIT_SIZE;

	/* Set display handler to priority if is not already */
	if (ipc->display == DH_CONNECTED) {
		send_display_controle_packet(ipc, CTRL_DH_PRIORITY, 0);
	}

	/* Wait for display handler to process the queue */
	/* We need to unlock the semaphore to let the display handler process the queue */
	sem_unlock(ipc->semid);
	while (1) {
		sem_lock(ipc->semid);
		queue_size = message_queue_size_get(ipc->msgid);
		if (queue_size < MSG_QUEUE_LIMIT_SIZE) {
			break ;
		}
		sem_unlock(ipc->semid);
		usleep(10000);
	}
}

void send_pdata_display(IPC *ipc, Player *player, u8 msg_type)
{
	u32 p_pos = get_board_index(player->pos);
	u32 p_target = get_board_index(player->target);
	u32 p_ally = get_board_index(player->ally_pos);
	u32 p_tid = player->team_id;
	u32 p_state = (u32)(player->state | msg_type);
	u32 p_sup = 0;

	if (msg_type == P_UPDATE_POS) {
		p_sup = get_board_index(player->next_pos);
	} else if (msg_type == P_DELETE) {
		p_sup = player->kill_by;
	}
	
	/* Hard build packet maybe do it cleaner in macro/function */
	u32 data[PDATA_LEN] = {(u32) 0, p_state , p_tid, p_pos, p_target, p_ally, p_sup};

	s8 ret = TRUE;

	/*
		Before trying to send pdata to display handler we need to check if the queue can support his size
		If not we need to give the display handler time to process the queue and clear it
		For this we need to unlock the semaphore and wait the display handler clear the queue
		But for that we need to prevent other player to just lock and block the display handler
	*/

	if (ipc->display) {
		if (message_queue_size_get(ipc->msgid) >= MSG_QUEUE_LIMIT_SIZE) {
			wait_for_display_handler_priority(ipc);
		}

		for (int i = 0; i < PDATA_LEN; ++i) {
			ret = send_msg(ipc, DISPLAY_HANDLER_CHAN, data[i], player->team_id);
			if (ret == FALSE) {
				ft_printf_fd(1, RED"Send pdata display failed, queue size %u\n"RESET, message_queue_size_get(ipc->msgid));
				break ;
			}
		}
	} 
}