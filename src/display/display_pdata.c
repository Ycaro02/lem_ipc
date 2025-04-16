# include "../../include/display.h"


/* @brief extract msg type from pdata first field */
char *get_player_strtype(u8 type)
{
	if (type == P_CREATE) {
		return ("CREATE");
	} else if (type == P_DELETE) {
		return ("DELETE");
	}
	return ("UNKNOWN");
}

/* @brief extract state from pdata first field */
char *get_player_strstate(u8 state)
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
char *get_vector_string(t_vec vec)
{
	char *brack_start = "[";
	char *brack_end = "]";

	// static char *out_of_board = "OUT OF BOARD";

	if (vector_cmp(vec, get_board_vec(OUT_OF_BOARD))) {
		return (ft_strdup("OUT OF BOARD"));
	}

	char *str = ft_strjoin_free(brack_start, ft_ultoa(vec.y), 's');
	str = ft_strjoin_free(str, brack_end, 'f');
	str = ft_strjoin_free(str, " ", 'f');

	str = ft_strjoin_free(str, brack_start, 'f');
	str = ft_strjoin_free(str, ft_ultoa(vec.x), 'a');
	str = ft_strjoin_free(str, brack_end, 'f');

	return (str);
}

/**
 * @brief put vector to str in rightband
 * @param game: game struct
 * @param str: first string to display
 * @param vdata: vector data to display
 * @param y: current y position to display
 * @return u32: new y position
*/
static u32 put_vectostr(Game *game, char *str, t_vec vdata, u32 y)
{
	char	*vec_str = get_vector_string(vdata);
	char	*tmp_str = ft_strjoin(str, " ");
	iVec2	pos = create_vector(y, RB_START_STR_X);

	write_text(game->h, str, game->h->font, pos, ORANGE2_INT);
	pos.x += get_str_pixel_len(str, game->h->font, GET_X);
	write_text(game->h, vec_str, game->h->font, pos, TURQUOISE_INT);
	free(tmp_str);
	free(vec_str);
	return (y);
}

/**
 * @brief put uint to str in rightband
 * @param game: game struct
 * @param str: first string to display
 * @param state: state string to display (if NULL display data, else display state string)
 * @param data: data to display (uint value and can be convert to string with ultoa call)
 * @param y: current y position to display
 * @return u32: new y position
*/
static u32 put_uint_tostr(Game *game, char *str, char *state, u32 data, u32 y)
{
	char	*tmp_str = NULL, *data_str = NULL;
	iVec2	pos = create_vector(y, RB_START_STR_X);
	
	tmp_str = ft_strjoin(str, " ");
	write_text(game->h, str, game->h->font, pos, ORANGE2_INT);
	pos.x += get_str_pixel_len(str, game->h->font, GET_X);
	free(tmp_str);

	if (!state) {
		data_str = ft_ultoa(data);
	} else {
		data_str = state;
	}

	write_text(game->h, data_str, game->h->font, pos, TURQUOISE_INT);

	if (!state) {
		free(data_str);
	}
	return (y);
}

/**
 *	@brief Display pdata node
 *	@param game: game struct
 *	@param pdata: pdata node to display
 *	@param y: current y position to display in pixel
*/
void display_pdata_node(Game *game, PlayerData *pdata, u32 y)
{
	u32 pad_y = get_str_pixel_len("T", game->h->font, GET_Y);
	char *str_state = get_player_strstate(GET_MSG_STATE(pdata[PDATA_STATE].sdata));

	y = put_vectostr(game, "Tile: ", pdata[PDATA_POS].vdata, y + 0);
	y = put_uint_tostr(game, "Team ID: ", NULL, pdata[PDATA_TID].sdata, y + pad_y);
	y = put_uint_tostr(game, "State: ", str_state, 0, y + pad_y);
	y = put_vectostr(game, "Position: ", pdata[PDATA_POS].vdata, y + pad_y);
	y = put_vectostr(game, "Target: ", pdata[PDATA_TARGET].vdata, y + pad_y);
	y = put_vectostr(game, "Ally: ", pdata[PDATA_ALLY].vdata, y + pad_y);
}
