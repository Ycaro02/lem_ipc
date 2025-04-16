
# include "../../include/display.h"

/* @brief Draw the board */
void sdl_draw_board(Game *game, SDLHandle *h, s8 empty_draw) {
	iVec2	tile_pos, scale;
	u32		color = 0xFFFFFFFF;

	scale.x = TILE_SIZE - 1;
	scale.y = scale.x;
	for (u32 y = 1; y < SCREEN_HEIGHT; ++y) {
		tile_pos.y = y;
		for (u32 x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			tile_pos.x = x;
			if (!empty_draw) {
				u32 tile_state = game->ipc->ptr[((x / TILE_SIZE) % BOARD_W) + ((y / TILE_SIZE) * BOARD_W)];
				color = tile_state == TILE_EMPTY ? 0xFFFFFFFF : get_new_color(tile_state).color;
			}
			draw_color_tile(h, tile_pos, scale, color);
			x += scale.x;
		}
		y += scale.y;
	}
}

/**
 * @brief Get the last team alive id
 * @param game Game structure
 * @return The last team alive id
*/
static uint32_t get_last_team_alive_id(Game *game) {
	if (game->team_data) {
		t_list *current = game->team_data;
		Team *team = current->content;
		return (team->tid);
	} else {
		ft_printf_fd(2, "No team data available in get last team alive id\n");
	}
	return (0);
}

/* @brief Display routine if we are connected */
static void display_routine(Game *game, SDLHandle *h) {
	static uint32_t last_team_id = 0;
	
	/* Lock sem */
	sem_lock(game->ipc->semid);
	game->sem_locked = TRUE;
	/* Check if player is selected */
	if (game->mouse_pos.y != UINT32_MAX && game->mouse_pos.x != UINT32_MAX) {
		game->player_selected = get_player_node(game->player_data, game->mouse_pos);
		game->mouse_pos = create_vector(UINT32_MAX, UINT32_MAX);
	}
	/* Pause game */
	set_playing_state(game->ipc->ptr, !game->pause);
	
	/* Update player number */
	game->player_nb = get_attached_processnb(game->ipc);
	/* Extract message from message queue if message received */
	if (extract_msg(game->ipc, DISPLAY_HANDLER_CHAN) != UINT32_MAX) {
		receive_player_data(game);
	}
	
	/* Extract priority packet */
	extract_priority_packet(game);

	/* Check for end of game (number of team) */
	s32 nb_team = ft_lstsize(game->team_data);
	if (nb_team != 0) {
		last_team_id = get_last_team_alive_id(game);
	}

	if (nb_team == 0 || game->player_nb == 2) {
 		char *str = ft_ultoa(last_team_id);
		if (str) {
			char *total_win_str = ft_strjoin("Winner team: ", str);
			if (total_win_str) {
				sdl_display_center_string(game, h, total_win_str, get_new_color(last_team_id).color);
				SDL_RenderPresent(h->renderer);
				free(total_win_str);
			}
			free(str);
		}
		sleep(2);
		g_game_run = 0;
		destroy_windows(game);
	}
	window_clear(h->renderer, U32_CLEAR_COLOR);
	sdl_draw_board(game, h, PLAYER_BOARD);
	display_righband(game, game->player_selected);
	/* Unlock sem */
	sem_unlock(game->ipc->semid);
	game->sem_locked = FALSE;
}



/* @brief Main display function */
static void sdl_main_display(Game *game, SDLHandle *h) {

	event_handler(game, h);

	if (game->ressource_state == ERROR_CASE) {
		handle_display_connect(game);
	} else {
		display_routine(game, h);
	}
	if (game->pause) {
		sdl_display_center_string(game, h, "PAUSE", TURQUOISE_INT);
	}
	SDL_RenderPresent(h->renderer);
}


/* @brief Main display loop */
int sdl_draw_loop(Game *game) {
	
	/* Handle signal */
	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		ft_printf_fd(2, "Can't catch SIGINT\n");
		return (-1);
	} else if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
		ft_printf_fd(2, "Can't catch SIGQUIT\n");
		return (-1);
	}

	while (g_game_run) {
		sdl_main_display(game, game->h);
		SDL_Delay(16);
	}
	return (TRUE);
}
