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
	uint32_t	type_state = pdata[PDATA_STATE].sdata;
	uint8_t 	type = GET_MSG_TYPE(type_state);
	// uint8_t 	state = GET_MSG_STATE(type_state);
	
	if (type == P_CREATE) {
		add_pdata_node(game, pdata);
		team_handling(&game->team_data, game->ipc->ptr, pdata[PDATA_TID].sdata, JOIN_TEAM);
		return ;
	}

	t_pdata *target_node = get_player_node(game->player_data, pdata[PDATA_POS].vdata);
	if (type == P_DELETE) {
		int8_t is_selected_node = 0;
		uint32_t kill_by = get_board_index(pdata[PDATA_SUPP].vdata);

		team_handling(&game->team_data, game->ipc->ptr, kill_by, UPDATE_KILL);
		// ft_printf_fd(2, RED"Player node delete kill by %u\n"RESET, kill_by);
		if (!target_node) {
			// ft_printf_fd(2, RED"Player node not found in DELETE case nothing todo (update kill counter maybe)\n"RESET);
			return ;
		}
		if (game->selected == target_node) {
			is_selected_node = 1;
		}
		ft_lst_remove_if(&game->player_data, target_node, free, is_same_node);
		team_handling(&game->team_data, game->ipc->ptr, pdata[PDATA_TID].sdata, REMOVE_TEAM);
		// team_handling(&game->team_data, game->ipc->ptr, pdata[PDATA_TID].sdata, REMOVE_TEAM);

		if (is_selected_node) {
			game->selected = NULL;
		}
		return ;
	}
	/* Update case here, just update pdata_pos for update_pos case*/
	if (type == P_UPDATE_POS) {
		pdata[PDATA_POS].vdata = pdata[PDATA_SUPP].vdata;		/* update PPOS */
	}
	if (target_node) {
		ft_memcpy((void *)target_node, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
	} else {
		add_pdata_node(game, pdata);
		team_handling(&game->team_data, game->ipc->ptr, pdata[PDATA_TID].sdata, JOIN_TEAM);
	}

}

/**
 *	@brief Extract and parse player data receive
 *	@param game: game struct
*/
void receive_player_data(t_game *game)
{
	t_pdata		pdata[PDATA_LEN] = INIT_PDATA_PACKET;
	uint32_t	ret = UINT32_MAX;
	uint8_t		count = 1; /* count = 1 cause if we call this we already receive the first 0 data start */

	do {
		ret = extract_msg(game->ipc, DISPLAY_HANDLER_ID);
		pdata[count].sdata = ret;
		if (count != PDATA_START) {
			if (count >= PDATA_POS) {
				pdata[count].vdata = get_board_vec(pdata[count].sdata);
			} 
		}
		++count;
		if (count >= PDATA_LEN) {
			handle_player_data(game, pdata);
			count = PDATA_START;
		}
	} while (ret != UINT32_MAX);
}

int8_t extract_controle_packet(t_game *game)
{
	uint32_t data[PDATA_LEN] = {0};
	uint32_t i = 0;
	int8_t ret = 0;

	sem_lock(game->ipc->semid);
	for (i = 0; i < PDATA_LEN; ++i) {
		data[i] = extract_msg(game->ipc, CONTROLE_DISPLAY_CHAN);
		if (data[i] != 0) {
			break;
		}
	}
	if (i == 0 && data[i] == UINT32_MAX) {
		ft_printf_fd(2, RED"Display handler packet not found, display error\n"RESET);
	} 
	else if (i == PDATA_LEN) {
		ft_printf_fd(2, GREEN"Display handler packet found\n"RESET);
		clear_msg_queue(game->ipc, UINT32_MAX);
		ret = 1;
	}
	// ft_printf_fd(2, CYAN"In display handler i val = %d, ret val %u\n"RESET, i, ret);
	sem_unlock(game->ipc->semid);
	return (ret);
}
