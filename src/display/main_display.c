# include "../../include/display.h"

/* Game run status, global to handle sigint */
int		g_game_run; 



/* @brief Initialize display */
int init_displayer(SDLHandle *h, Player *player, int argc, char **argv) {
	if (argc != 1) {
		ft_printf_fd(2, "Usage: no args required %s\n", argv[0]);
		return (-1);
	}
	player->team_id = UINT32_MAX;
	
	if (!(h->font = load_font(ARIAL_FONT_PATH, INFO_FONT_SIZE))) {
		ft_printf_fd(2, "Failed to load font %s\n", ARIAL_FONT_PATH);
		return (-1);
	} else if (!(h->big_font = load_font(ARIAL_FONT_PATH, BIG_FONT_SIZE))) {
		ft_printf_fd(2, "Failed to load big font %s\n", ARIAL_FONT_PATH);
		unload_font(h->font);
		return (-1);
	}
	return (0);
}

/* @brief Destroy windows/display */
int destroy_windows(Game *game) {

	unload_font(game->h->font);
	unload_font(game->h->big_font);
	window_close(game->h->window, game->h->renderer);

	if (game->player_data) {
		ft_lstclear(&game->player_data, free);
	}
	if (game->team_data) {
		ft_lstclear(&game->team_data, free_team);
	}

	if (game->ressource_state != ERROR_CASE) {
		if (game->pause) { // if game is paused unlock it
			set_playing_state(game->ipc->ptr, TRUE);
		}
		ft_printf_fd(1, CYAN"Display exit, resend controle packet\n"RESET); 
		send_display_controle_packet(game->ipc, CTRL_DH_WAITING_TO_CONNECT, DISPLAY_HANDLER_ID);
		detach_shared_memory(game->ipc);
		if (game->sem_locked) {
			sem_unlock(game->ipc->semid);
		}
	}
	free(game);
	exit(0);
}


s32 event_handler(Game *game, SDLHandle *h) {
	SDL_Event event;
	
	s32 x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	while (SDL_PollEvent(&event)) {
		mouse_position_update(h, x, y);	
		if (event.type == SDL_QUIT || is_key_pressed(event, SDLK_ESCAPE)) {
			destroy_windows(game);
		} else if (is_key_pressed(event, SDLK_SPACE) || is_key_pressed(event, SDLK_p)) {
			game->pause = !game->pause; 
		} else if (is_left_click_down(event)) {
			game->mouse_pos = get_click_tile(h->mouse);
		}
	}

	return (TRUE);
}

/* @brief Main function for display handler */
int main(int argc, char **argv)  {
	IPC			ipc = {};
	Player		player = {};
	Game		*game = getGame();
	if (!game) {
		ft_printf_fd(2, "Game structure alloc failed\n");
		return (1);
	}

	g_game_run = TRUE;
	game->ipc = &ipc;
	game->h = create_sdl_handle("LemIPC Vizualiser", SCREEN_HEIGHT, SCREEN_WIDTH);
	if (!game->h) {
		return (1);
	} else if (init_displayer(game->h, &player, argc, argv) != 0) {
		goto display_error;
	} 
	game->ressource_state = ERROR_CASE;
	if (sdl_draw_loop(game) == -1) {
		goto display_error;
	}

	display_error:
		window_close(game->h->window, game->h->renderer);
		free(game);
		return (1);

}
