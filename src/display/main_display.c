# include "../../include/display.h"

int		g_game_run; 

t_teamcolor get_new_color(uint32_t team_id) 
{
	t_teamcolor team_color[] = {
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
	uint32_t idx = 0;
	for (uint32_t i = 0; COLOR_MAX ; i++) {
			if (i == (team_id - 1) % COLOR_MAX) {
				idx = i;
				break ;
			}
		}
	// ft_printf_fd(2, CYAN"idx %d\n"RESET, idx);
	return (team_color[idx]);
}


/* @brief Initialize display */
int init_display(t_player *player, int argc, char **argv)
{
	if (argc != 1) {
		ft_printf_fd(2, "Usage: no args required %s\n", argv[0]);
		return (-1);
	}
	player->team_id = UINT32_MAX;
	return (0);
}

/* @brief Destroy windows/display */
int destroy_windows(t_game *game)
{
	mlx_destroy_image(game->mlx, game->img.image);
	mlx_destroy_image(game->mlx, game->right_band.image);
	mlx_destroy_image(game->mlx, game->pause_btn);
	mlx_destroy_window(game->mlx, game->win);
	mlx_destroy_display(game->mlx);
	if (game->player_data) {
		ft_lstclear(&game->player_data, free);
	}
	if (game->team_data) {
		ft_lstclear(&game->team_data, free_team);
	}
	if (!game->sem_lock) {
		ft_printf_fd(2, PURPLE"Display lock sem\n"RESET);
		sem_lock(game->ipc->semid);
	}
	ft_printf_fd(2, CYAN"Display exit, resend controle packet\n"RESET); 
	send_display_controle_packet(game->ipc);
	detach_shared_memory(game->ipc);
	sem_unlock(game->ipc->semid);
	free(game->mlx);
	free(game);
	exit(0);
}


/* @brief skip x utils, to know how much pixel we need to skip after string */
int skip_x(char *str) {
	return (ft_strlen(str) * CHAR_TOPIXEL);
}

/* @brief Display team info lst */
static int	display_team_info(t_game *game, t_team *team, uint32_t pad_y, uint32_t str_sizemax)
{
	uint32_t start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	uint32_t y = 20U + pad_y;
	uint32_t x = start_x;
	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "ID : ");
	x += skip_x("ID : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->strid); 
	x += (str_sizemax * CHAR_TOPIXEL);
	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "NB : ");
	x += skip_x("NB : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->strsize); 
	x += ((str_sizemax) * CHAR_TOPIXEL);
	mlx_string_put(game->mlx, game->win, x, y, team->color.color, "K : ");
	x += skip_x("K : ");
	mlx_string_put(game->mlx, game->win, x, y, GREEN_INT, team->kill_str); 
	return (0);
}


/* TO_REWORK read new dataset */
void display_righband(t_game *game, t_pdata *pdata)
{
	uint8_t count = 0;
	uint32_t y = 20U;
	char *player_remain = ft_ultoa(get_attached_processnb(game->ipc) - 1U);
	
	/* reset right band */
	mlx_put_image_to_window(game->mlx, game->win, game->right_band.image, SCREEN_WIDTH - RIGHTBAND_WIDTH, 0);

	/* Display btn */
	mlx_put_image_to_window(game->mlx, game->win, game->pause_btn, (int)(SCREEN_WIDTH - RIGHTBAND_WIDTH), SCREEN_HEIGHT - (TILE_SIZE * 2));

	mlx_string_put(game->mlx, game->win, START_STR_X, y, CYAN_INT, PLAYER_REMAIN);
	if (player_remain) {
		mlx_string_put(game->mlx, game->win, START_STR_X + skip_x(PLAYER_REMAIN), y, RED_INT, player_remain);
		free(player_remain);
	}
	y += PAD_YTEAM;

	if (game->team_data) {
		for (t_list *current = game->team_data; current && count < BEST_TEAM; current = current->next) {
			display_team_info(game, current->content, y, 3);
			y += PAD_YTEAM;
			++count;
		}
	}

	if (pdata) {
		display_pdata_node(game, pdata, y + PAD_YTEAM);
	}
}

/* @brief key press handler */
int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game); /* maybe need to check sem value and lock it to detash mem */
	return (0);
}

/* @brief Draw board */
static void draw_board(t_game *game)
{
	int color = 0;

	for (uint32_t y = 1; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			uint32_t x_compute =  ((x / TILE_SIZE) % BOARD_W);
			uint32_t y_compute = ((y / TILE_SIZE) * BOARD_W);   
			uint32_t idx = x_compute + y_compute;
			uint32_t tile_state = game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : get_new_color(tile_state).color;
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((uint32_t *)game->img.data)[x + (y * (SCREEN_WIDTH - RIGHTBAND_WIDTH))] = color;
			}
		}
	}
}

// # define PAUSE_BTN_IDX 11U

#define PAUSE_BTN_IDX (SCREEN_HEIGHT / (TILE_SIZE * 2) - 1)

/* Main display function called in mlx loop hook */
int main_display(void *vgame)
{
	t_game		*game = vgame;
	uint32_t	tmp = 0U;


	/* If game is paused sem already lock */
	if (!game->pause) {
		sem_lock(game->ipc->semid);
		game->sem_lock = 1;
	}
	/* Check for game pause handle click */
	if (game->mouse_pos.x == UINT32_MAX && game->mouse_pos.y != UINT32_MAX) {
		ft_printf_fd(2, YELLOW"Click on btn %u, x: %u, y: %u\n"RESET, game->mouse_pos.y, game->mouse_pos.x, game->mouse_pos.y);
		ft_printf_fd(2, YELLOW"PAUSE_BTN_IDX %u\n"RESET, PAUSE_BTN_IDX);
		if (game->mouse_pos.y == PAUSE_BTN_IDX) {
			game->pause = !(game->pause);
		}
		game->mouse_pos.y = UINT32_MAX;
		// ft_printf_fd(2, CYAN"Game pause %d\n"RESET, game->pause);
	} else if (game->mouse_pos.y != UINT32_MAX && game->mouse_pos.x != UINT32_MAX) {
		game->selected = get_player_node(game->player_data, game->mouse_pos);
		game->mouse_pos = create_vector(UINT32_MAX, UINT32_MAX);
	}

	/* Update player number */
	game->player_nb = get_attached_processnb(game->ipc);
	/* Update player data lst */
	tmp = extract_msg(game->ipc, UINT32_MAX);
	if (tmp != UINT32_MAX) {
		receive_player_data(game);
	}

	/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
	if (game->player_nb <= 3) {
		// ft_printf_fd(2, PURPLE"Shutdown display\n"RESET);
		g_game_run = 0;
		destroy_windows(game);
	}

	/* Draw board in image */
	draw_board(game);
	/* Unlock sem if game is not paused */
	if (!game->pause) {
		sem_unlock(game->ipc->semid);
		game->sem_lock = 0;
	}

	
	/* Display image (flush) */
	mlx_put_image_to_window(game->mlx, game->win, game->img.image, 0, 0);

	
	/* Display old team list to remove/rework to read new data list */
	display_righband(game, game->selected);
	/* 1/10 sec */
	// usleep(100000);
	// usleep(10000);
	return (0);
}

/* @brief Init display */
int8_t init_mlx(t_game *game) 
{
	int8_t	packet_extract = 0; 
	int		endian = 0;

	game->mlx = mlx_init();
	if (!game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	game->img.image = mlx_new_image(game->mlx, SCREEN_WIDTH - RIGHTBAND_WIDTH, SCREEN_HEIGHT);
	game->right_band.image = mlx_new_image(game->mlx, RIGHTBAND_WIDTH, SCREEN_HEIGHT - (TILE_SIZE * 2));
	// ft_printf_fd(1, CYAN"SCREEN WIDTH %u SCREEN HEIGHT %u\n"RESET, SCREEN_WIDTH, SCREEN_HEIGHT);
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


	int btn_width = (int)RIGHTBAND_WIDTH;
	int btn_height = (int)(TILE_SIZE * 2);
	game->pause_btn = mlx_xpm_file_to_image(game->mlx, PAUSE_BTN_ASSET, &btn_width, &btn_height);
	if (!game->pause_btn) {
		ft_printf_fd(2, "mlx_xpm_file_to_image %s failed\n", PAUSE_BTN_ASSET);
		return (ERROR_CASE);
	}


	/* Extract controle packet */
	packet_extract = extract_controle_packet(game);
	if (!packet_extract) {
		ft_printf_fd(2, RED"Display handler packet not found, exit\n"RESET);
		return (ERROR_CASE);
	}


	mlx_hook(game->win, 2, 1L, key_hooks_press, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, destroy_windows, game);
	mlx_mouse_hook(game->win, check_mouse, game);
	// mlx_loop_hook(game->mlx, display_board_stdout, game);
	mlx_loop_hook(game->mlx, main_display, game);
	mlx_loop(game->mlx);
	return (0);

}

/* @brief Main function for display handler */
int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	t_game		*game = ft_calloc(sizeof(t_game), 1);

	game->ipc = &ipc;
	game->player_nb = 0;

	if (init_display(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, DISPLAY_HANDLER) == ERROR_CASE\
		|| init_mlx(game) == ERROR_CASE) {
		free(game);
		return (1);
	}
	return (0);
}

/* CLear pixel buff */
// size_t len = sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT);
// ft_printf_fd(2, RED"len : %u, sizeof %u\n"RESET, len, sizeof(game->img.data));
// ft_bzero(game->img.data, len);

/* Debug img load */
// game->pause_btn = mlx_new_image(game->mlx, btn_width, btn_height);
// ft_printf_fd(2, PURPLE"Pause btn GIVEN before btn_width %d btn_height %d\n"RESET, btn_width, btn_height);
// game->pause_btn->width = (int)RIGHTBAND_WIDTH;
// game->pause_btn->height = (int)(TILE_SIZE * 2);
// ft_printf_fd(2, GREEN"Pause btn GIVEN after btn_width %d btn_height %d\n"RESET, btn_width, btn_height);
// ft_printf_fd(2, CYAN"Pause btn GOT btn_width %d btn_height %d\n"RESET, game->pause_btn->width, ((t_img *)game->pause_btn)->height);


/* @brief first display function, display board on stdout */
// int display_board_stdout(t_game *game) {
// 	sem_lock(game->ipc->semid);
// 	if (get_attached_processnb(game->ipc) <= 1) {
// 		ft_printf_fd(2, RED"Shutdown display Board stdout\n"RESET);
// 		g_game_run = 0;
// 		detach_shared_memory(game->ipc);
// 		sem_unlock(game->ipc->semid);
// 		destroy_windows(game);
// 	}
// 	display_uint16_array(game->ipc->ptr);
// 	sem_unlock(game->ipc->semid);
// 	usleep(10000); /* 1/10 sec */
// 	return (0);
// }