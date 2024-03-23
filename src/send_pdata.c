# include "../include/lem_ipc.h"


void display_packet(uint32_t *data)
{
	ft_printf_fd(2, CYAN"\n-------------------------Send packet --------------------------------------\n"RESET);
	ft_printf_fd(2, GREEN"start: |%u| "RESET, data[PDATA_START]);
	ft_printf_fd(2, GREEN"state: |%u| "RESET, data[PDATA_STATE]);
	ft_printf_fd(2, GREEN"tid: |%u| "RESET, data[PDATA_TID]);
	ft_printf_fd(2, GREEN"pos: |%u| "RESET, data[PDATA_POS]);
	ft_printf_fd(2, GREEN"target: |%u| "RESET, data[PDATA_TARGET]);
	ft_printf_fd(2, GREEN"ally: |%u|"RESET, data[PDATA_ALLY]);
	ft_printf_fd(2, GREEN"supp: |%u|"RESET, data[PDATA_SUPP]);
	ft_printf_fd(2, CYAN"\n-------------------------Packet end---------------------------------------\n"RESET);

}

void send_display_controle_packet(t_ipc *ipc)
{
	// uint32_t data[PDATA_LEN] = {(uint32_t) 0, (uint32_t)0, (uint32_t)0, (uint32_t)0, (uint32_t)0, (uint32_t)0, (uint32_t)0};
	uint32_t	data[PDATA_LEN] = DISPLAY_CTRL_PACKET;

	for (int i = 0; i < PDATA_LEN; ++i) {
		send_msg(ipc, CONTROLE_DISPLAY_CHAN, data[i]);
	}
}

int8_t display_handler_state(t_ipc *ipc)
{
	uint32_t	data[PDATA_LEN] = INVALID_CTRL_PACKET;
	int			i;
	int8_t		ret = 0;

	for (i = 0; i < PDATA_LEN; ++i) {
		data[i] = extract_msg(ipc, CONTROLE_DISPLAY_CHAN);
		if (data[i] != 0) {
			break;
		}
	}
	if (i == 0 && data[i] == UINT32_MAX) {
		// ft_printf_fd(2, GREEN"Display handler packet not found, start send player status\n"RESET);
		ret = 1;
	} 
	else if (i == PDATA_LEN){
		// ft_printf_fd(2, GREEN"Display handler packet found by client, resend it\n"RESET);
		send_display_controle_packet(ipc);
	}
	return (ret);
}

void send_pdata_display(t_ipc *ipc, t_player *player, uint8_t msg_type)
{
	uint32_t p_pos = get_board_index(player->pos);
	uint32_t p_target = get_board_index(player->target);
	uint32_t p_ally = get_board_index(player->ally_pos);
	uint32_t p_tid = player->team_id;
	uint32_t p_state = (uint32_t)(player->state | msg_type);
	uint32_t p_sup = 0;

	if (msg_type == P_UPDATE_POS) {
		p_sup = get_board_index(player->next_pos);
	} else if (msg_type == P_DELETE) {
		p_sup = player->kill_by;
	}
	/* Hard build packet maybe do it cleaner in macro/function */
	uint32_t data[PDATA_LEN] = {(uint32_t) 0, p_state , p_tid, p_pos, p_target, p_ally, p_sup};

	// display_packet(data);
	if (player->display) {
		for (int i = 0; i < PDATA_LEN; ++i) {
			send_msg(ipc, UINT32_MAX, data[i]);
		}
	}
}