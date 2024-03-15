# include "../include/display.h"

int		g_game_run; 
t_game	*g_game;


t_teamcolor get_new_color(uint32_t current_team_nb, uint32_t team_id) 
{
    t_teamcolor team_color[] = {
		{"Red", 0xFF0000},
		{"Blue", 0x0000FF},
		{"Green", 0x00FF00},
		{"Yellow", 0xFFFF00},
		{"Purple", 0x800080},
		{"Cyan", 0x00FFFF},
		{"Orange", 0xFFA500},
		{"Pink", 0xFFC0CB},
		{"Brown", 0xA52A2A},
		{"Black", 0x000000},
		{(void*)0, 0}/* Terminator. */
	};
	(void)team_id;
	int idx = 0;
	for (uint32_t i = 0; i < current_team_nb; ++i) {
			if (i == current_team_nb) {
				idx = i % 10U;
				break ;
			}
		}
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


static int	rgb_to_int(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

int	display_team_info(t_list *team)
{
	// char	*nb = "koala";
	int y = 20U;
	(void)team;
	int red = rgb_to_int(255, 10, 10);
	int x = SCREEN_WIDTH - (TILE_SIZE * 5) + 5U;
	mlx_string_put (g_game->mlx, g_game->win, x, y, red, "Team Info : ");
	// mlx_string_put (game->mlx, game->win, SCREEN_WIDTH + 10, 100, 0x0F0, "TESTTTTTTTTTTTTTTTTTTTTTTTTTTT : ");
	// mlx_string_put (game->mlx, game->win, SCREEN_WIDTH + 80, 32, 0xFFF, nb);
	return (0);
}

int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game);
	return (0);
}


int boardmlx_display() {
	int color = 0;

	sem_lock(g_game->ipc->semid);

	/* Update team info lst */
	if (!build_list_number_team(&g_game->team, g_game->ipc->ptr)) {
		ft_printf_fd(2, RED"Error: build_list_number_team\n"RESET);
	}

	/* CLear pixel buff */
	size_t len = sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT);
	// ft_printf_fd(2, RED"len : %u, sizeof %u\n"RESET, len, sizeof(g_game->img.data));
	ft_bzero(g_game->img.data, len);

	if (get_attached_processnb(g_game->ipc) > 30) {
		ft_printf_fd(2, RED"DISPLAY TEAM INFO\n"RESET);
		display_team_info(g_game->team);
	}


	/* Check if only one team left */
	if (g_game->ipc->ptr[TEAM_NB] == 1) {
		ft_printf_fd(2, PURPLE"Shutdown display team number [NB] won\n"RESET);
		g_game_run = 0;
		sem_unlock(g_game->ipc->semid);
		destroy_windows();
	}

	/* Draw board */
	for (uint32_t y = 1; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < (SCREEN_WIDTH - RIGHTBAND_WIDTH); ++x) {
			uint32_t idx = ((x / TILE_SIZE) % BOARD_W) + ((y / TILE_SIZE) * BOARD_W);
			uint32_t tile_state = g_game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : tile_state % 2 ? 0x0000FF : 0xFF0000;
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((uint32_t *)g_game->img.data)[x + (y * SCREEN_WIDTH)] = color;
			}
		}
	}
	mlx_put_image_to_window(g_game->mlx, g_game->win, g_game->img.image, 0, 0);
	sem_unlock(g_game->ipc->semid);
	usleep(100000); /* 1/10 sec */
	return (0);
}

int8_t init_mlx() {
	int endian = 0;
	g_game->mlx = mlx_init();
	if (!g_game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	g_game->win = mlx_new_window(g_game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	g_game->img.image = mlx_new_image(g_game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	ft_printf_fd(1, CYAN"SCREEN WIDTH %u SCREEN HEIGHT %u\n"RESET, SCREEN_WIDTH, SCREEN_HEIGHT);
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
	g_game->team_nb = 0;

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