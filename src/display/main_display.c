# include "../../include/display.h"

// #include "../../include/handle_sdl.h"

int		g_game_run; 

TeamColor get_new_color(u32 team_id)  {
	static TeamColor team_color[] = {
		{"Red", 0xFF0000FF},
		{"Blue", 0x6699FFFF},
		{"Yellow",0xFFCC00FF},
		{"Green", 0x00FF00FF},
		{"Pink", 0xFF00CCFF},
		{"Purple", 0xCC33FFFF},
		{"Cyan", 0x00FFFFFF},
		{"Orange", 0xFF9900FF},
		{"Brown", 0x99991EFF},
		{"Grey", 0x999999FF},
		{"Red Light", 0xCC0000FF},
		{"Pink Light", 0xFFCCCCFF},
		{"Yellow Light", 0xFFFF00FF},
		{"Green Light", 0xCCFF00FF},
		{"Green Dark", 0x358000FF},
		{"Blue Light", 0x0000CCFF},
		{"Blue Light", 0x99CCFFFF},
		{"Cyan Light", 0xCCFFFFFF},
		{"Purple Light", 0x9900CCFF},
		{"Purple Light", 0xCC99FFFF},
		{"Brown Light", 0x996600FF},
		{"Brown Dark", 0x666600FF},
		{"Grey Dark", 0x666666FF},
		{"Grey Light", 0xCCCCCCFF},
		{"Green Light", 0x79CC3DFF},
		{"Grey Light", 0xCCCC99FF},
	};
	u32 idx = 0;

	for (u32 i = 0; COLOR_MAX ; i++) {
		if (i == (team_id - 1) % COLOR_MAX) {
			idx = i;
			break ;
		}
	}
	return (team_color[idx]);
}


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


/* @brief Compute total kill stored in list of team */
static u32 compute_total_kill(t_list *team_lst) {
	u32 total_kill = 0;
	for (t_list *current = team_lst; current; current = current->next) {
		total_kill += ((Team *)current->content)->kill;
	}

	return (total_kill);
}

/* @brief Compute total team size stored in list of team */
// static u32 compute_total_team_size(t_list *team_lst) {
// 	u32 total_size = 0;
// 	for (t_list *current = team_lst; current; current = current->next) {
// 		total_size += ((Team *)current->content)->tsize;
// 	}

// 	return (total_size);
// }

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
int	display_team_info(Game *game, Team *team, u32 y) {
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

/* @brief Extract controle priority packet */
void extract_priority_packet(Game *game){
	u32 data[PDATA_LEN] = {UINT32_MAX};
	u32 i = 0;
	for (i = 0; i < PDATA_LEN; ++i) {
		data[i] = extract_msg(game->ipc, CONTROLE_DISPLAY_CHAN);
		if (data[i] != CTRL_DH_PRIORITY) {
			break;
		}
	}
}

/* @brief Get the static game context */
Game *getGame(void) {
	static Game *game = NULL;

	if (!game) {
		game = ft_calloc(sizeof(Game), 1);
	}
	return (game);
}

/* @brief Signal handler to handle resource */
void sig_handler(int signum) {
	Game *game = getGame();

	if (signum == SIGINT) {
		destroy_windows(game);
	}
}


s8 is_key_pressed(SDL_Event event, s32 key) {
	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == key);
}

s8 is_left_click_down(SDL_Event event) {
	return (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT);
}

void mouse_position_update(SDLHandle *h, int x, int y) {
	h->mouse.x = x;
	h->mouse.y = y;
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

/* @brief Display a grey rectangle */
void sdl_pause_rectangle_display(SDLHandle *h) {
    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_SetRenderDrawColor(h->renderer, 128, 128, 128, 128);
    SDL_RenderFillRect(h->renderer, &rect);
}

/* @brief Display pause text and rectangle */
void sdl_pause_display(Game *game, SDLHandle *h) {
	iVec2	pos = create_vector(SCREEN_HEIGHT >> 1, 0);
	u32		font_height = get_str_pixel_len("PAUSE", h->big_font, GET_Y);
	u32		font_width = get_str_pixel_len("PAUSE", h->big_font, GET_X);

	/* Display a grey fog rectangle */
	sdl_pause_rectangle_display(h);

	/* Center the text */
	pos.y -= (font_height >> 1);
	pos.x = ((SCREEN_WIDTH - RIGHTBAND_WIDTH) >> 1) - (font_width >> 1);
	write_text(h, "PAUSE", game->h->big_font, pos, TURQUOISE_INT);
}

/* @brief Handle display connect */
void handle_display_connect(Game *game) {
	game->ressource_state = init_game(game->ipc, IPC_NAME, DISPLAY_HANDLER);
	if (game->ressource_state != ERROR_CASE) {
		if (extract_controle_packet(game) == -1) {
			ft_printf_fd(2, "Failed to extract controle packet\n");
			destroy_windows(game);
		}
	}
	window_clear(game->h->renderer, U32_CLEAR_COLOR);
	sdl_draw_board(game, game->h, EMPTY_BOARD);
}

/* @brief Display routine if we are connected */
void display_routine(Game *game, SDLHandle *h) {
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
	/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
	// if (game->player_nb <= 3) {
	s32 nb_team = ft_lstsize(game->team_data);
	// ft_printf_fd(1, YELLOW"Game end close windows nb team: %d\n"RESET, ft_lstsize(game->team_data));
	if (nb_team == 0) {
		g_game_run = 0;
		destroy_windows(game);
	}
	// }
	window_clear(h->renderer, U32_CLEAR_COLOR);
	sdl_draw_board(game, h, PLAYER_BOARD);
	display_righband(game, game->player_selected);
	/* Unlock sem */
	sem_unlock(game->ipc->semid);
	game->sem_locked = FALSE;
}

/* @brief Main display function */
void sdl_main_display(Game *game, SDLHandle *h) {

	event_handler(game, h);

	if (game->ressource_state == ERROR_CASE) {
		handle_display_connect(game);
	} else {
		display_routine(game, h);
	}
	if (game->pause) {
		sdl_pause_display(game, h);
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
	game->h = create_sdl_handle("LemIPC", SCREEN_HEIGHT, SCREEN_WIDTH);
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
