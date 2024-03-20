# include "../include/display.h"

int		g_game_run; 

t_teamcolor get_new_color(uint32_t current_team_nb, uint32_t team_id) 
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


	(void)current_team_nb;
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


int destroy_windows(t_game *game)
{
	mlx_destroy_image(game->mlx, game->img.image);
	mlx_destroy_window(game->mlx, game->win);
	mlx_destroy_display(game->mlx);
	if (game->team) {
		ft_lstclear(&game->team, free_team);
	}
	free(game->mlx);
	free(game);
	ft_printf_fd(1, "Exit MLX\n");
	exit(0);
}

int display_board_stdout(t_game *game) {
	sem_lock(game->ipc->semid);
	if (get_attached_processnb(game->ipc) <= 1) {
		ft_printf_fd(2, RED"Shutdown display\n"RESET);
		g_game_run = 0;
		detach_shared_memory(game->ipc);
		sem_unlock(game->ipc->semid);
		destroy_windows(game);
	}
	display_uint16_array(game->ipc->ptr);
	sem_unlock(game->ipc->semid);
	usleep(10000); /* 1/10 sec */
	return (0);
}

/* to put in header file */

void		mystring_put(t_xvar *xvar,t_win_list *win,int x,int y,int color,char *string)
{
   XGCValues	xgcv;
   
   xgcv.foreground = mlx_int_get_good_color(xvar,color);
   XChangeGC(xvar->display,win->gc,GCForeground,&xgcv);
   XDrawString(xvar->display,win->window,win->gc,x,y,string,strlen(string));
}


int skip_x(char *str) {
	return (ft_strlen(str) * CHAR_TOPIXEL_SKIP);
}

static int	display_team_info(t_game *game, t_team *team, uint32_t pad_y, uint32_t str_sizemax)
{
	uint32_t start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	uint32_t y = 20U + pad_y;

	uint32_t x = start_x;

	mlx_string_put(game->mlx, game->win, x, y, team->data.color, "ID : ");
	x += skip_x("ID : ");
	mlx_string_put(game->mlx, game->win, x, y, CYAN_INT, team->strid);
	x += ((str_sizemax + 2) * CHAR_TOPIXEL_SKIP);
	mlx_string_put(game->mlx, game->win, x, y, team->data.color, "REMAIN : ");
	x += skip_x("REMAIN : ");
	mlx_string_put(game->mlx, game->win, x, y, CYAN_INT, team->strsize);
	
	return (0);
}

static uint32_t	get_max_strsize(t_list *list)
{
	t_list		*tmp = list;
	uint32_t	max = 0;
	uint32_t	tmp_val = 0;

	while (tmp) {
		tmp_val = ft_strlen(((t_team *)tmp->content)->strsize);
		if (tmp_val > max) {
			max = tmp_val;
		}
		tmp = tmp->next;
	}
	return (max);
}

void display_teamlist(t_game *game, t_list *list)
{
	t_list *tmp = list;
	uint32_t y = 15U;
	char *player_remain = ft_ultoa(get_attached_processnb(game->ipc) - 1U);

	uint32_t str_size_max = get_max_strsize(list);
	mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U), y, CYAN_INT, PLAYER_REMAIN);
	mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U) + skip_x(PLAYER_REMAIN), y, RED_INT, player_remain);
	y += PAD_YTEAM;
	mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U), y, CYAN_INT, "TEAM INFO : ");

	while (tmp) {
		display_team_info(game, tmp->content, y, str_size_max);
		tmp = tmp->next;
		y += PAD_YTEAM;
	}
}

int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game); /* maybe need to check sem value and lock it to detash mem */
	return (0);
}


static int get_team_color(t_list *team, uint32_t team_id)
{
	t_list *tmp = team;
	while (tmp) {
		if (((t_team *)tmp->content)->tid == team_id) {
			return (((t_team *)tmp->content)->data.color);
		}
		tmp = tmp->next;
	}
	return (0);
}


static void draw_board(t_game *game)
{
	int color = 0;

	for (uint32_t y = 1; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < SCREEN_WIDTH - RIGHTBAND_WIDTH; ++x) {
			uint32_t x_compute =  ((x / TILE_SIZE) % BOARD_W);
			uint32_t y_compute = ((y / TILE_SIZE) * BOARD_W);   
			uint32_t idx = x_compute + y_compute;
			uint32_t tile_state = game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : get_team_color(game->team, tile_state);
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((uint32_t *)game->img.data)[x + (y * SCREEN_WIDTH)] = color;
			}
		}
	}

}

int boardmlx_display(void *vgame)
{
	t_game *game = vgame;

	sem_lock(game->ipc->semid);

	/* Update team info lst */
	if ((uint32_t) get_attached_processnb(game->ipc) != game->player_nb) {
		ft_lstclear(&game->team, free_team);
		if (!build_list_number_team(&game->team, game->ipc->ptr)) {
			ft_printf_fd(2, RED"Error: build_list_number_team\n"RESET);
		}
		game->player_nb = get_attached_processnb(game->ipc);
	}

	/* CLear pixel buff */
	// size_t len = sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT);
	// ft_printf_fd(2, RED"len : %u, sizeof %u\n"RESET, len, sizeof(game->img.data));
	// ft_bzero(game->img.data, len);

	/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
	if (game->ipc->ptr[TEAM_NB] <= 1 || get_attached_processnb(game->ipc) <= 3) {
		ft_printf_fd(2, PURPLE"Shutdown display team number [NB] won\n"RESET);
		g_game_run = 0;
		detach_shared_memory(game->ipc);
		sem_unlock(game->ipc->semid);
		destroy_windows(game);
	}

	/* Draw board */
	draw_board(game);
	sem_unlock(game->ipc->semid);

	mlx_put_image_to_window(game->mlx, game->win, game->img.image, 0, 0);
	if (game->team) {
		display_teamlist(game, game->team);
	}
	// usleep(10000); /* 1/10 sec */
	return (0);
}

int8_t init_mlx(t_game *game) 
{
	int endian = 0;
	game->mlx = mlx_init();
	if (!game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	game->img.image = mlx_new_image(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	// ft_printf_fd(1, CYAN"SCREEN WIDTH %u SCREEN HEIGHT %u\n"RESET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!game->win || !game->img.image) {
		ft_printf_fd(2, "mlx_new_window or image failed\n");
		return (ERROR_CASE);
	}
	game->img.data = mlx_get_data_addr(game->img.image, &game->img.bpp,
			&game->img.width, &endian);
	if (!game->img.data) {
		ft_printf_fd(2, "mlx_get_data_addr failed\n");
		return (ERROR_CASE);
	}

	mlx_hook(game->win, 2, 1L, key_hooks_press, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, destroy_windows, game);
	// mlx_loop_hook(game->mlx, display_board_stdout, game);
	// mlx_loop_hook(game->mlx, display_team_info, game);
	mlx_loop_hook(game->mlx, boardmlx_display, game);
	// mlx_loop_hook(game->mlx, display_board_stdout, game);
	mlx_loop(game->mlx);
	return (0);

}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	t_game		*game = malloc(sizeof(t_game));

	game->ipc = &ipc;
	game->team = NULL;
	game->player_nb = 0;

	if (init_display(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, DISPLAY_HANDLER) == ERROR_CASE\
		|| init_mlx(game) == ERROR_CASE) {
		return (1);
	}
	

	// sem_lock(ipc.semid);
	// ft_printf_fd(2, RED"Lem-ipc Display Handler end\n"RESET, player.team_id);	
	// sem_unlock(ipc.semid);

	return (0);
}