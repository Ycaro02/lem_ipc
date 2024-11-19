# include "../../include/display.h"

int		g_game_run; 

TeamColor get_new_color(u32 team_id)  {
	static TeamColor team_color[] = {
		{"Red", 0xFF0000},
		{"Blue", 0x6699FF},
		{"Yellow", 0xFFCC00},
		{"Green", 0x00FF00},
		{"Pink", 0xFF00CC},
		{"Purple", 0xCC33FF},
		{"Cyan", 0x00FFFF},
		{"Orange", 0xFF9900},
		{"Brown", 0x99991E},
		{"Grey", 0x999999},
		{"Red Light", 0xCC0000},
		{"Pink Light", 0xFFCCCC},
		{"Yellow Light", 0xFFFF00},
		{"Green Light", 0xCCFF00},
		{"Green Dark", 0x358000},
		{"Blue Light", 0x0000CC},
		{"Blue Light", 0x99CCFF},
		{"Cyan Light", 0xCCFFFF},
		{"Purple Light", 0x9900CC},
		{"Purple Light", 0xCC99FF},
		{"Brown Light", 0x996600},
		{"Brown Dark", 0x666600},
		{"Grey Dark", 0x666666},
		{"Grey Light", 0xCCCCCC},
		{"Green Light", 0x79CC3D},
		{"Grey Light", 0xCCCC99},
	};
	u32 idx = 0;

	for (u32 i = 0; COLOR_MAX ; i++) {
		if (i == (team_id - 1) % COLOR_MAX) {
			idx = i;
			break ;
		}
	}
	// ft_printf_fd(2, CYAN"idx %d\n"RESET, idx);
	return (team_color[idx]);
}


/* @brief Initialize display */
int init_display(Player *player, int argc, char **argv) {
	if (argc != 1) {
		ft_printf_fd(2, "Usage: no args required %s\n", argv[0]);
		return (-1);
	}
	player->team_id = UINT32_MAX;
	return (0);
}

/* @brief Destroy windows/display */
int destroy_windows(Game *game) {
	mlx_destroy_image(game->mlx, game->img.image);
	mlx_destroy_image(game->mlx, game->right_band.image);
	// mlx_destroy_image(game->mlx, game->pause_btn);
	mlx_destroy_window(game->mlx, game->win);
	mlx_destroy_display(game->mlx);
	
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
		sem_unlock(game->ipc->semid);
	}
	free(game->mlx);
	free(game);
	exit(0);
}


/* @brief skip x utils, to know how much pixel we need to skip after string */
int skip_x(char *str) {
	return (ft_strlen(str) * CHAR_TOPIXEL);
}

static u32 compute_total_kill(t_list *team_lst) {
	u32 total_kill = 0;
	for (t_list *current = team_lst; current; current = current->next) {
		total_kill += ((Team *)current->content)->kill;
	}

	return (total_kill);
}

static u32 compute_total_team_size(t_list *team_lst) {
	u32 total_size = 0;
	for (t_list *current = team_lst; current; current = current->next) {
		total_size += ((Team *)current->content)->tsize;
	}

	return (total_size);
}

static void display_another_info(Game *game, char *str, u32 y, u32 digit) {
	u32 start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	u32 x = start_x;
	mlx_string_put(game->mlx, game->win, x, y, YELLOW_INT, str);
	x += skip_x(str);

	char *total_kill_str = ft_itoa(digit);
	mlx_string_put(game->mlx, game->win, x, y, PINK_INT, total_kill_str);
	free(total_kill_str);
}

/* @brief Display team info lst */
static int	display_team_info(Game *game, Team *team, u32 pad_y, u32 str_sizemax) {
	u32 start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	u32 y = 20U + pad_y;
	u32 x = start_x;

	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "ID : ");
	x += skip_x("ID : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->strid); 
	x += (str_sizemax * CHAR_TOPIXEL);
	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "NB : ");
	x += skip_x("NB : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->strsize); 
	x += ((str_sizemax + 2) * CHAR_TOPIXEL);
	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "K : ");
	x += skip_x("K : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->kill_str); 
	return (0);
}


void reset_rightband(Game *game) {
	mlx_put_image_to_window(game->mlx, game->win, game->right_band.image, SCREEN_WIDTH - RIGHTBAND_WIDTH, 0);
}

/* TO_REWORK read new dataset */
void display_righband(Game *game, PlayerData *pdata) {
	u8 count = 0;
	u32 y = 20U;
	char *player_remain = ft_ultoa(get_attached_processnb(game->ipc) - 1U);
	

	reset_rightband(game);

	mlx_string_put(game->mlx, game->win, START_STR_X, y, CYAN_INT, PLAYER_REMAIN);
	if (player_remain) {
		mlx_string_put(game->mlx, game->win, START_STR_X + skip_x(PLAYER_REMAIN), y, RED_INT, player_remain);
		free(player_remain);
	}
	y += PAD_YTEAM;

	if (game->team_data) {
		for (t_list *current = game->team_data; current && count < MAX_TEAM_DISPLAY; current = current->next) {
			display_team_info(game, current->content, y, 3);
			y += PAD_YTEAM;
			++count;
		}
	}

	/* Display player data */
	y += (PAD_YTEAM * 2);
	display_another_info(game, "Total Kill : ", y, compute_total_kill(game->team_data) + game->kill_from_remove_team);
	y += PAD_YTEAM;
	display_another_info(game, "Total Team Size : ", y, compute_total_team_size(game->team_data));

	if (pdata) {
		display_pdata_node(game, pdata, y + PAD_YTEAM);
	}
}

/* @brief key press handler */
int	key_hooks_press(int keycode, Game *game) {
	if (keycode == ESC) {
		destroy_windows(game); /* maybe need to check sem value and lock it to detash mem */
	}  else if (keycode == SPACE) {
		game->space_state = !game->space_state;
	}
	// else {
	// 	ft_printf_fd(2, "Keycode %d\n", keycode);
	// }
	return (0);
}

static void draw_empty_board(Game *game) {
	for (u32 y = 1; y < SCREEN_HEIGHT; ++y) {
		for (u32 x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((u32 *)game->img.data)[x + (y * (SCREEN_WIDTH - RIGHTBAND_WIDTH))] = 0xFFFFFF;
			}
		}
	}
}

/* @brief Draw board */
static void draw_board(Game *game) {
	int color = 0;
	u32 tile_state = 0;

	for (u32 y = 1; y < SCREEN_HEIGHT; ++y) {
		for (u32 x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			// u32 x_compute =  ((x / TILE_SIZE) % BOARD_W);
			// u32 y_compute = ((y / TILE_SIZE) * BOARD_W);   
			// u32 idx = x_compute + y_compute;
			tile_state = game->ipc->ptr[((x / TILE_SIZE) % BOARD_W) + ((y / TILE_SIZE) * BOARD_W)];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : get_new_color(tile_state).color;
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((u32 *)game->img.data)[x + (y * (SCREEN_WIDTH - RIGHTBAND_WIDTH))] = color;
			}
		}
	}
}

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

Game *geGame(void) {
	static Game *game = NULL;

	if (!game) {
		game = ft_calloc(sizeof(Game), 1);
	}
	return (game);
}

// signal handler funct
void sig_handler(int signum) {
	Game *game = geGame();

	if (signum == SIGINT) {
		destroy_windows(game);
	}
}

/* Main display function called in mlx loop hook */
int main_display(void *vgame) {
	Game		*game = vgame;
	u32			tmp = 0;

	/* Handle signal */
	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		ft_printf_fd(2, "Can't catch SIGINT\n");
		return (-1);
	}

	game->pause = game->space_state;

	if (game->ressource_state == ERROR_CASE) {

		draw_empty_board(game);

		/* Try to init game again */
		game->ressource_state = iniGame(game->ipc, IPC_NAME, DISPLAY_HANDLER);
		if (game->ressource_state != ERROR_CASE) {
			/* Extract controle packet */
			extract_controle_packet(game);
		}
		reset_rightband(game);
	} else {
		/* Lock sem */
		sem_lock(game->ipc->semid);

		/* Check if player is selected */
		if (game->mouse_pos.y != UINT32_MAX && game->mouse_pos.x != UINT32_MAX) {
			game->player_selected = gePlayer_node(game->player_data, game->mouse_pos);
			game->mouse_pos = create_vector(UINT32_MAX, UINT32_MAX);
		}

		/* Pause game */
		set_playing_state(game->ipc->ptr, !game->pause);

		/* Update player number */
		game->player_nb = get_attached_processnb(game->ipc);

		/* Extract message from message queue */
		tmp = extract_msg(game->ipc, DISPLAY_HANDLER_CHAN);
		if (tmp != UINT32_MAX) { /* if we receive a message */
			receive_player_data(game);
		}

		/* Extract priority packet */
		extract_priority_packet(game);

		/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
		if (game->player_nb <= 3) {
			g_game_run = 0;
			destroy_windows(game);
		}

		/* Draw board in image */
		draw_board(game);

		/* Unlock sem */
		sem_unlock(game->ipc->semid);

		/* Display old team list to remove/rework to read new data list */
		display_righband(game, game->player_selected);
	}
	
	display_another_info(game, "Game Paused : ", (BOARD_H * TILE_SIZE) - TILE_SIZE * 2, game->pause ? 1 : 0);


	/* Display image (flush) */
	mlx_put_image_to_window(game->mlx, game->win, game->img.image, 0, 0);


	return (0);
}

/* @brief Init display */
s8 init_mlx(Game *game) 
{
	int		endian = 0;

	game->mlx = mlx_init();
	if (!game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	game->img.image = mlx_new_image(game->mlx, SCREEN_WIDTH - RIGHTBAND_WIDTH, SCREEN_HEIGHT);
	game->right_band.image = mlx_new_image(game->mlx, RIGHTBAND_WIDTH, SCREEN_HEIGHT - (TILE_SIZE * 2));
	if (!game->win || !game->img.image || !game->right_band.image) {
		ft_printf_fd(2, "mlx_new_window or image failed\n");
		return (ERROR_CASE);
	}
	game->img.data = mlx_get_data_addr(game->img.image, &game->img.bpp,
			&game->img.width, &endian);
	game->right_band.data = mlx_get_data_addr(game->right_band.image, &game->right_band.bpp,
			&game->right_band.width, &endian);
	if (!game->img.data || !game->right_band.data) {
		ft_printf_fd(2, "mlx_get_data_addr failed\n");
		return (ERROR_CASE);
	}

	/* Extract controle packet */
	// if (game->ressource_state != ERROR_CASE) {
	// }

	mlx_hook(game->win, 2, 1L, key_hooks_press, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, destroy_windows, game);
	mlx_mouse_hook(game->win,check_mouse, game);
	mlx_loop_hook(game->mlx, main_display, game);

	mlx_loop(game->mlx);
	return (0);

}

#include "../../include/handle_sdl.h"

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
			window_close(h->window, h->renderer);
			free(h);
			exit(1); // toremove
		} else if (is_key_pressed(event, SDLK_SPACE)) {
			ft_printf_fd(1, "Space\n");
			// pause here
		} else if (is_left_click_down(event)) {
			game->mouse_pos = get_click_tile(h->mouse);
		}
	}

	return (TRUE);
}

void sdl_draw_empty_board(SDLHandle *h) {
	iVec2 tile_pos, scale;
	u32		color = 0xFFFFFF;

	for (u32 y = 1; y < SCREEN_HEIGHT; ++y) {
		for (u32 x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			tile_pos.y = y;
			tile_pos.x = x;
			scale.x = TILE_SIZE;
			scale.y = TILE_SIZE;
			draw_color_tile(h, tile_pos, scale, color);
			y += TILE_SIZE;
			x += TILE_SIZE;
			// if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
			// 	((u32 *)game->img.data)[x + (y * (SCREEN_WIDTH - RIGHTBAND_WIDTH))] = 0xFFFFFF;
			// }
		}
	}
}

/* @brief Main function for display handler */
int main(int argc, char **argv) 
{
	Game		*game = geGame();
	if (!game) {
		ft_printf_fd(2, "Malloc failed\n");
		return (1);
	}

	(void)argc, (void)argv;
	SDLHandle *h = create_sdl_handle("LemIPC", SCREEN_WIDTH, SCREEN_WIDTH);
	if (!h) {
		return (1);
	}
	while (1) {
		window_clear(h->renderer);
		//sdl_draw_empty_board(h);
		SDL_RenderPresent(h->renderer);
		event_handler(game, h);
	}

	// IPC		ipc = {};
	// Player	player = {};


	// game->ipc = &ipc;
	// game->player_nb = 0;
	// if (init_display(&player, argc, argv) != 0) {
	// 	goto display_error;
	// }

	// game->ressource_state = iniGame(&ipc, IPC_NAME, DISPLAY_HANDLER);
	// if (init_mlx(game) == ERROR_CASE) {
	// 	goto display_error;
	// }	
	// return (0);

	// display_error:
	// 	free(game);
	// 	return (1);

}
