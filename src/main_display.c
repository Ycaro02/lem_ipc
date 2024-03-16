# include "../include/display.h"

int		g_game_run; 
t_game	*g_game;

t_teamcolor get_new_color(uint32_t current_team_nb, uint32_t team_id) 
{
	t_teamcolor team_color[] = {
		{"Red", 0xFF0000},
		{"Orange", 0xFF9900},
		{"Yellow", 0xFFCC00},
		{"Green", 0x00FF00},
		{"Blue", 0x6699FF},
		{"Purple", 0xCC33FF},
		{"Brown", 0x99991E},
		{"Grey", 0x999999},
		{"Pink", 0xFF00CC},
		{"Red", 0xCC0000},
		{"Pink", 0xFFCCCC},
		{"Yellow", 0xFFFF00},
		{"Green", 0xCCFF00},
		{"Green", 0x358000},
		{"Blue", 0x0000CC},
		{"Blue", 0x99CCFF},
		{"Cyan", 0x00FFFF},
		{"Cyan", 0xCCFFFF},
		{"Purple", 0x9900CC},
		{"Purple", 0xCC99FF},
		{"Brown", 0x996600},
		{"Brown", 0x666600},
		{"Grey", 0x666666},
		{"Grey", 0xCCCCCC},
		{"Green", 0x79CC3D},
		{"Grey", 0xCCCC99},
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


int destroy_windows()
{
	mlx_destroy_image(g_game->mlx, g_game->img.image);
	mlx_destroy_window(g_game->mlx, g_game->win);
	mlx_destroy_display(g_game->mlx);
	if (g_game->team) {
		ft_lstclear(&g_game->team, free_team);
	}
	free(g_game->mlx);
	free(g_game);
	ft_printf_fd(1, "Exit MLX\n");
	exit(0);
}

int game_display() {
	sem_lock(g_game->ipc->semid);
	if (get_attached_processnb(g_game->ipc) <= 1) {
		ft_printf_fd(2, RED"Shutdown display\n"RESET);
		g_game_run = 0;
		sem_unlock(g_game->ipc->semid);
		destroy_windows();
	}
	display_uint16_array(g_game->ipc->ptr);
	sem_unlock(g_game->ipc->semid);
	usleep(10000); /* 1/10 sec */
	return (0);
}

/* to put in header file */


// static int	rgb_to_int(int r, int g, int b)
// {
// 	return ((r << 16) | (g << 8) | b);
// }


void		mystring_put(t_xvar *xvar,t_win_list *win,int x,int y,int color,char *string)
{
   XGCValues	xgcv;
   
   xgcv.foreground = mlx_int_get_good_color(xvar,color);
   XChangeGC(xvar->display,win->gc,GCForeground,&xgcv);
   XDrawString(xvar->display,win->window,win->gc,x,y,string,strlen(string));
}


int skip_x(char *str)
{
	return (ft_strlen(str) * 6);
}

# define PAD_YTEAM 25U

int	display_team_info(t_team *team, uint32_t pad_y)
{
	uint32_t start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	uint32_t y = 20U + pad_y;

	uint32_t x = start_x;

	mlx_string_put(g_game->mlx, g_game->win, x, y, team->data.color, "TEAM ID : ");
	x += skip_x("TEAM ID : ");
	mlx_string_put(g_game->mlx, g_game->win, x, y, CYAN_INT, team->strid);
	
	x = start_x;
	y += PAD_YTEAM;
	mlx_string_put(g_game->mlx, g_game->win, x, y, team->data.color, "TEAM SIZE : ");
	x += skip_x("TEAM SIZE : ");
	mlx_string_put(g_game->mlx, g_game->win, x, y, CYAN_INT, team->strsize);
	
	return (0);
}

void display_teamlist(t_list *list)
{
	t_list *tmp = list;
	uint32_t y = 20U;

	mlx_string_put(g_game->mlx, g_game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U), y, CYAN_INT, "TEAM INFO : ");
	y += PAD_YTEAM;
	while (tmp) {
		display_team_info(tmp->content, y);
		tmp = tmp->next;
		y += (PAD_YTEAM * 2);
	}
}

int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game);
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

int boardmlx_display() 
{
	int color = 0;

	sem_lock(g_game->ipc->semid);

	/* Update team info lst */

	if ((uint32_t) get_attached_processnb(g_game->ipc) != g_game->player_nb) {
		ft_lstclear(&g_game->team, free_team);
		if (!build_list_number_team(&g_game->team, g_game->ipc->ptr)) {
			ft_printf_fd(2, RED"Error: build_list_number_team\n"RESET);
		}
		g_game->player_nb = get_attached_processnb(g_game->ipc);
	}

	/* CLear pixel buff */
	// size_t len = sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT);
	// ft_printf_fd(2, RED"len : %u, sizeof %u\n"RESET, len, sizeof(g_game->img.data));
	// ft_bzero(g_game->img.data, len);

	// if (g_game->team) {
	// 	display_teamlist(g_game->team);
	// }


	/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
	if (g_game->ipc->ptr[TEAM_NB] == 1 || get_attached_processnb(g_game->ipc) <= 3) {
		ft_printf_fd(2, PURPLE"Shutdown display team number [NB] won\n"RESET);
		g_game_run = 0;
		sem_unlock(g_game->ipc->semid);
		destroy_windows();
	}

	/* Draw board */
	for (uint32_t y = 1; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < SCREEN_WIDTH - RIGHTBAND_WIDTH; ++x) {
			uint32_t x_compute =  ((x / TILE_SIZE) % BOARD_W);
			uint32_t y_compute = ((y / TILE_SIZE) * BOARD_W);   
			uint32_t idx = x_compute + y_compute;
			uint32_t tile_state = g_game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : get_team_color(g_game->team, tile_state);
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((uint32_t *)g_game->img.data)[x + (y * SCREEN_WIDTH)] = color;
			}
		}
	}
	mlx_put_image_to_window(g_game->mlx, g_game->win, g_game->img.image, 0, 0);
	if (g_game->team) {
		display_teamlist(g_game->team);
	}
	sem_unlock(g_game->ipc->semid);
	usleep(100000); /* 1/10 sec */
	return (0);
}

int8_t init_mlx() 
{
	int endian = 0;
	g_game->mlx = mlx_init();
	if (!g_game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	g_game->win = mlx_new_window(g_game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	g_game->img.image = mlx_new_image(g_game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	// ft_printf_fd(1, CYAN"SCREEN WIDTH %u SCREEN HEIGHT %u\n"RESET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!g_game->win || !g_game->img.image) {
		ft_printf_fd(2, "mlx_new_window or image failed\n");
		return (ERROR_CASE);
	}
	g_game->img.data = mlx_get_data_addr(g_game->img.image, &g_game->img.bpp,
			&g_game->img.width, &endian);
	if (!g_game->img.data) {
		ft_printf_fd(2, "mlx_get_data_addr failed\n");
		return (ERROR_CASE);
	}

	mlx_hook(g_game->win, 2, 1L, key_hooks_press, g_game);
	mlx_hook(g_game->win, DestroyNotify, StructureNotifyMask, destroy_windows, g_game);
	// mlx_loop_hook(g_game->mlx, game_display, g_game);
	// mlx_loop_hook(g_game->mlx, display_team_info, g_game);
	mlx_loop_hook(g_game->mlx, boardmlx_display, g_game);
	// mlx_loop_hook(g_game->mlx, game_display, g_game);
	mlx_loop(g_game->mlx);
	return (0);

}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	
	g_game = malloc(sizeof(t_game));
	g_game->ipc = &ipc;
	g_game->team = NULL;
	g_game->player_nb = 0;

	if (init_display(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, DISPLAY_HANDLER) == ERROR_CASE\
		|| init_mlx() == ERROR_CASE) {
		return (1);
	}
	

	sem_lock(ipc.semid);
	ft_printf_fd(2, RED"Lem-ipc Display Handler end\n"RESET, player.team_id);	
	sem_unlock(ipc.semid);

	return (0);
}