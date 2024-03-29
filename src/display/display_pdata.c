# include "../../include/display.h"


/* @brief extract msg type from pdata first field */
static char *get_player_strtype(uint8_t type)
{
	if (type == P_CREATE) {
		return ("CREATE");
	} else if (type == P_DELETE) {
		return ("DELETE");
	} else if (type == P_UPDATE) {
		return ("UPDATE");
	}
	return ("UNKNOWN");
}

/* @brief extract state from pdata first field */
static char *get_player_strstate(uint8_t state)
{
	if (state == S_WAITING) {
		return ("WAITING");
	} else if (state == S_TRACKER) {
		return ("TRACKER");
	} else if (state == S_FOLLOWER) {
		return ("FOLLOWER");
	}
	return ("UNKNOWN");
}

/* Convert vector to formated allocated str */
static char *get_vector_string(t_vec vec)
{
	char *brack_start = "[";
	char *brack_end = "]";

	char *str = ft_strjoin_free(brack_start, ft_ultoa(vec.y), 's');
	str = ft_strjoin_free(str, brack_end, 'f');

	str = ft_strjoin_free(str, brack_start, 'f');
	str = ft_strjoin_free(str, ft_ultoa(vec.x), 'a');
	str = ft_strjoin_free(str, brack_end, 'f');

	// sprintf(str, "[%u][%u]", vec.y, vec.x);
	return (str);
}

/**
 * @brief put vector to str in rightband
 * @param game: game struct
 * @param str: first string to display
 * @param vdata: vector data to display
 * @param y: current y position to display
 * @return uint32_t: new y position
*/
static uint32_t put_vectostr(t_game *game, char *str, t_vec vdata, uint32_t y)
{
	char *vec_str = get_vector_string(vdata);
	char *tmp_str = ft_strjoin_free(str, vec_str, 's');

	mlx_string_put(game->mlx, game->win, START_STR_X, y, CYAN_INT, tmp_str);
	free(tmp_str);
	return (PAD_YTEAM);
}

/**
 * @brief put uint to str in rightband
 * @param game: game struct
 * @param str: first string to display
 * @param state: state string to display (if NULL display data, else display state string)
 * @param data: data to display (uint value and can be convert to string with ultoa call)
 * @param y: current y position to display
 * @return uint32_t: new y position
*/
static uint32_t put_uint_tostr(t_game *game, char *str, char *state, uint32_t data, uint32_t y)
{
	char *tmp_str = NULL;
	if (state) {
		tmp_str = ft_strjoin(str, state);
	} else {
		tmp_str = ft_strjoin_free(str, ft_ultoa(data), 's');
	}
	mlx_string_put(game->mlx, game->win, START_STR_X, y, CYAN_INT, tmp_str);
	free(tmp_str);
	return (PAD_YTEAM);
}


/* @brief display pdata list on stdout */
void display_pdata_lst(t_list *player_lst)
{
	t_list *tmp = player_lst;
	t_pdata *pdata = NULL;
	while (tmp) {
		pdata = tmp->content;
		for (int i = 1; i < PDATA_LEN; i++) {
			ft_printf_fd(2, YELLOW"%s: "RESET, pdata[i].name);
			if (i >= PDATA_POS) {
				ft_printf_fd(2, CYAN"[%u] [%u]\n"RESET, pdata[i].vdata.y, pdata[i].vdata.x);
			} else if (i == PDATA_STATE) {
				ft_printf_fd(2, PURPLE" Msg type %s, Player State: %s\n"RESET, get_player_strtype(GET_MSG_TYPE(pdata[i].sdata))\
					, get_player_strstate(GET_MSG_STATE(pdata[i].sdata)));
			} else {
				ft_printf_fd(2, CYAN"%u\n"RESET, pdata[i].sdata);
			}
		}
		tmp = tmp->next;
	}
}

/**
 *	@brief Display pdata node
 *	@param game: game struct
 *	@param pdata: pdata node to display
 *	@param y: current y position to display in pixel
*/
void display_pdata_node(t_game *game, t_pdata *pdata, uint32_t y)
{
	char *str_state = get_player_strstate(GET_MSG_STATE(pdata[PDATA_STATE].sdata));

	y += put_vectostr(game, "Tile: ", pdata[PDATA_POS].vdata, y);
	y += put_uint_tostr(game, "Team ID: ", NULL, pdata[PDATA_TID].sdata, y);
	y += put_uint_tostr(game, "State: ", str_state, 0, y);
	y += put_vectostr(game, "Position: ", pdata[PDATA_POS].vdata, y);
	y += put_vectostr(game, "Target: ", pdata[PDATA_TARGET].vdata, y);
	y += put_vectostr(game, "Ally: ", pdata[PDATA_ALLY].vdata, y);
}
