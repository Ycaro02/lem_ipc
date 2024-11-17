# include "../../include/display.h"

/* @brief just compare addr */
int is_same_node(void *node, void *target) {
	return (node == target);
}

/**
 *	@brief Get player node from list
 *	@param lst: list to search in
 *	@param target: target vector to search
 *	@return void *: player node
*/
void *get_player_node(t_list *lst, t_vec target)
{
	t_pdata *pdata = NULL;
	for (t_list *current = lst; current; current = current->next) {
		pdata = current->content;
		// ft_printf_fd(2, PURPLE"GetPlayerNode: vdaata [%u] [%u]\n", pdata[PDATA_POS].vdata.y, pdata[PDATA_POS].vdata.x);
		// ft_printf_fd(2, GREEN"GetPlayerNode: target [%u] [%u]\n", target.y, target.x);
		if (vector_cmp(pdata[PDATA_POS].vdata, target)) {
			return (current->content);
		}
	}
	return (NULL);
}


static void add_pdata_node(t_game *game, t_pdata *pdata)
{
	t_pdata *tmp = ft_calloc(sizeof(t_pdata), PDATA_LEN);
	if (tmp) {
		tmp = ft_memcpy((void *)tmp, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
		ft_lstadd_back(&game->player_data, ft_lstnew(tmp));
	}
}

/**
 *	@brief Handle player data, update linked list (create update and delete node gestion)
 *	@param game: game struct
 *	@param pdata: player data
*/
static void handle_player_data(t_game *game, t_pdata *pdata)
{
	t_pdata	*player_node = NULL;
	u8		type = GET_MSG_TYPE(pdata[PDATA_STATE].sdata);

	// u8 	state = GET_MSG_STATE(type_state);
	// t_vec pos = pdata[PDATA_POS].vdata;
	// char *type_msg = (type == P_CREATE) ? "CREATE" : (type == P_UPDATE_POS) ? "UPDATE" : (type == P_DELETE) ? "DELETE" : "UNKNOWN";
	// ft_printf_fd(1, "Player msgtype |%s|, team %u posV [%u|%u] posS [%u]\n", type_msg, pdata[PDATA_TID].sdata, pos.y, pos.x, pdata[PDATA_POS].sdata);

	if (type == P_CREATE) {
		add_pdata_node(game, pdata);
		team_handling(game, pdata[PDATA_TID].sdata, JOIN_TEAM);
		return ;
	}

	player_node = get_player_node(game->player_data, pdata[PDATA_POS].vdata);
	if (type == P_DELETE) {

		team_handling(game, get_board_index(pdata[PDATA_SUPP].vdata), UPDATE_KILL);
		if (!player_node) {
			return ;
		} else if (game->player_selected == player_node) {
			game->player_selected = NULL;
		}
		ft_lst_remove_if(&game->player_data, player_node, free, is_same_node);
		team_handling(game, pdata[PDATA_TID].sdata, REMOVE_TEAM);
		return ;
	}
	/* Update case here, just update pdata_pos for update_pos case*/
	if (type == P_UPDATE_POS) {
		pdata[PDATA_POS].vdata = pdata[PDATA_SUPP].vdata;		/* update PPOS */
	}
	if (player_node) { /* if target found just update it */
		ft_memcpy((void *)player_node, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
	} 
}

/**
 *	@brief Extract and parse player data receive
 *	@param game: game struct
*/
void receive_player_data(t_game *game)
{
	t_pdata		pdata[PDATA_LEN] = INIT_PDATA_PACKET;
	u32	ret = UINT32_MAX;
	u8		count = 1; /* count = 1 cause if we call this we already receive the first 0 data start */

	pdata[PDATA_START].sdata = 0;

	do {
		ret = extract_msg(game->ipc, DISPLAY_HANDLER_CHAN);
		pdata[count].sdata = ret;
		if (count != PDATA_START) {
			if (count >= PDATA_POS) {
				pdata[count].vdata = get_board_vec(pdata[count].sdata);
			} 
		}
		++count;
		if (count == PDATA_LEN) {
			handle_player_data(game, pdata);
			count = PDATA_START;
		}
	} while (ret != UINT32_MAX);
	// ft_printf_fd(1, GREEN"All Player data received-> queue size %u\n"RESET, message_queue_size_get(game->ipc->msgid));
}

s8 extract_controle_packet(t_game *game)
{
	u32 data[PDATA_LEN] = {UINT32_MAX};
	u32 i = 0;
	s8 ret = 0;

	sem_lock(game->ipc->semid);
	for (i = 0; i < PDATA_LEN; ++i) {
		data[i] = extract_msg(game->ipc, CONTROLE_DISPLAY_CHAN);
		// if (data[i] != 0) {
		if (data[i] != CTRL_DH_WAITING_TO_CONNECT && data[i] != CTRL_DH_PRIORITY) {
			break;
		}
	}
	if (i == 0 && data[i] == UINT32_MAX) {
		ft_printf_fd(2, RED"Display handler packet not found, display error\n"RESET);
	} 
	else if (i == PDATA_LEN) {
		// ft_printf_fd(2, GREEN"Display handler packet found\n"RESET);
		// clear_msg_queue(game->ipc, CONTROLE_DISPLAY_CHAN);
		ret = 1;
	}
	// ft_printf_fd(2, CYAN"In display handler i val = %d, ret val %u\n"RESET, i, ret);
	sem_unlock(game->ipc->semid);
	return (ret);
}
