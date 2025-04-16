# include "../../include/display.h"

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

	(void)signum;
	// ft_printf_fd(1, YELLOW"Signal received %d\n"RESET, signum);
	destroy_windows(game);
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


/* @brief Display a grey rectangle */
void sdl_pause_rectangle_display(SDLHandle *h) {
    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_SetRenderDrawColor(h->renderer, 128, 128, 128, 128);
    SDL_RenderFillRect(h->renderer, &rect);
}

/* @brief Display pause text and rectangle */
void sdl_display_center_string(Game *game, SDLHandle *h, char *str, uint32_t color) {
	iVec2	pos = create_vector(SCREEN_HEIGHT >> 1, 0);
	u32		font_height = get_str_pixel_len(str, h->big_font, GET_Y);
	u32		font_width = get_str_pixel_len(str, h->big_font, GET_X);

	/* Display a grey fog rectangle */
	sdl_pause_rectangle_display(h);

	/* Center the text */
	pos.y -= (font_height >> 1);
	pos.x = ((SCREEN_WIDTH - RIGHTBAND_WIDTH) >> 1) - (font_width >> 1);
	write_text(h, str, game->h->big_font, pos, color);
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
	SDL_RenderPresent(game->h->renderer);
	/* Wait for player send message */
	sleep(2);
}

