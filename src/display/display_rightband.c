# include "../../include/display.h"

/* @brief Compute total kill stored in list of team */
static u32 compute_total_kill(t_list *team_lst) {
	u32 total_kill = 0;
	for (t_list *current = team_lst; current; current = current->next) {
		total_kill += ((Team *)current->content)->kill;
	}

	return (total_kill);
}

/**
 * @brief Display Specific info in rightband
 * @param game: game struct
 * @param str: string to display
 * @param y: y position to display
 * @param digit: digit to display after the string
 */
static void display_another_info(Game *game, char *str, u32 y, u32 digit) {
	u32 start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	u32 x = start_x;
	
	iVec2 pos = create_vector(y, x);


	write_text(game->h, str, game->h->font, pos, TURQUOISE_INT);
	// mlx_string_put(game->mlx, game->win, x, y, YELLOW_INT, str);
	x += get_str_pixel_len(str, game->h->font, GET_X);

	char *total_kill_str = ft_itoa(digit);
	pos = create_vector(y, x);

	write_text(game->h, total_kill_str, game->h->font, pos, TURQUOISE_INT);
	// mlx_string_put(game->mlx, game->win, x, y, PINK_INT, total_kill_str);
	free(total_kill_str);
}

/**
 * @brief Display basic team info in rightband
 * @param game: game struct
 * @param team: team struct
 * @param y: y position to display
 */
static int	display_team_info(Game *game, Team *team, u32 y) {
	iVec2	pos = create_vector(y, RB_START_STR_X);

	write_text(game->h, "ID : ", game->h->font, pos, team->color.color);

	pos.x += get_str_pixel_len("ID : ", game->h->font, GET_X);
	write_text(game->h, team->strid, game->h->font, pos, TURQUOISE_INT);

	pos.x += get_str_pixel_len(team->strid, game->h->font, GET_X);
	write_text(game->h, "NB : ", game->h->font, pos, team->color.color);

	pos.x += get_str_pixel_len("NB : ", game->h->font, GET_X);
	write_text(game->h, team->strsize, game->h->font, pos, TURQUOISE_INT);	
	
	pos.x += get_str_pixel_len(team->strsize, game->h->font, GET_X);
	write_text(game->h, "K : ", game->h->font, pos, team->color.color);

	pos.x += get_str_pixel_len("K : ", game->h->font, GET_X);
	write_text(game->h, team->kill_str, game->h->font, pos, TURQUOISE_INT);
	return (0);
}


/**
 * @brief Display the rightband with player data
 * @param game: game struct
 * @param pdata: player data selected to display
 */
void display_righband(Game *game, PlayerData *pdata) {
	char	*player_remain = ft_ultoa(get_attached_processnb(game->ipc) - 1U);
	iVec2	pos = create_vector(RB_START_STR_Y, RB_START_STR_X);
	u32		pad_y = get_str_pixel_len("T", game->h->font, GET_Y);
	u8		count = 0;

	write_text(game->h, "Player Remain: ", game->h->font, pos, CYAN_INT);

	if (player_remain) {
		pos.x += get_str_pixel_len("Player Remain: ", game->h->font, GET_X);
		write_text(game->h, player_remain, game->h->font, pos, RED_INT);
		free(player_remain);
	}
	pos.y += (pad_y + pad_y);
	if (game->team_data) {
		for (t_list *current = game->team_data; current && count < MAX_TEAM_DISPLAY; current = current->next) {
			display_team_info(game, current->content, pos.y);
			pos.y += pad_y;
			++count;
		}
	}

	pos.y += pad_y;
	display_another_info(game, "Total Kill : ", pos.y, compute_total_kill(game->team_data) + game->kill_from_remove_team);

	// pos.y += pad_y;
	// display_another_info(game, "Total Team Size : ", pos.y, compute_total_team_size(game->team_data));

	if (pdata) {
		display_pdata_node(game, pdata, (pos.y + pad_y + pad_y));
	}


}