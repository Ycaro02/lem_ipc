# include "../include/display.h"

/* @brief just compare addr */
static int is_same_node(void *node, void *target) {
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
		// ft_printf_fd(2, CYAN"Player data create\n"RESET);
		t_pdata *tmp = ft_calloc(sizeof(t_pdata), PDATA_LEN);
		ft_memcpy((void *)tmp, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
		ft_lstadd_back(&game->player_data, ft_lstnew(tmp));
		return ;
	}

	t_pdata *target_node = get_player_node(game->player_data, pdata[PDATA_POS].vdata);
	if (type == P_DELETE) {
		// ft_printf_fd(2, RED"Player data delete\n"RESET);
		uint32_t kill_by = get_board_index(pdata[PDATA_SUPP].vdata);
		ft_printf_fd(2, RED"Player node delete kill by %u\n"RESET, kill_by);
		if (!target_node) {
			ft_printf_fd(2, RED"Player node not found in DELETE case nothing todo (update kill counter maybe)\n"RESET);
			return ;
		}
		int8_t is_selected_node = 0;
		if (game->selected == target_node) {
			is_selected_node = 1;
		}
		ft_lst_remove_if(&game->player_data, target_node, free, is_same_node);
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
		ft_printf_fd(2, CYAN"Player node not found in UPDATE case create node %u\n"RESET, pdata[PDATA_TID].sdata);
		t_pdata *tmp = ft_calloc(sizeof(t_pdata), PDATA_LEN);
		ft_memcpy((void *)tmp, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
		ft_lstadd_back(&game->player_data, ft_lstnew(tmp));
	}

}

/**
 *	@brief Extract and parse player data receive
 *	@param game: game struct
*/
void receive_player_data(t_game *game)
{
	t_pdata		pdata[PDATA_LEN] = INIT_MSG_PACK;
	uint32_t	ret = UINT32_MAX;
	uint8_t		count = 1; /* count = 1 cause if we call this we already receive the first 0 data start */

	do {
		ret = extract_msg(game->ipc, UINT32_MAX);
		pdata[count].sdata = ret;
		if (count != PDATA_START) {
			if (count >= PDATA_POS) {
				pdata[count].vdata = get_board_pos(pdata[count].sdata);
			} 
		}
		++count;
		if (count >= PDATA_LEN) {
			handle_player_data(game, pdata);
			count = PDATA_START;
		}
	} while (ret != UINT32_MAX);
}