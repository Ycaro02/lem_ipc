# include "../include/display.h"

int		g_game_run; 
t_game	*g_game;

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


int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game);
	return (0);
}

int boardmlx_display() {
	sem_lock(g_game->ipc->semid);
	uint32_t pixel_x = 0, pixel_y = 0;
	int color = 0;
	
	if (g_game->ipc->ptr[TEAM_NB] == 1) {
		ft_printf_fd(2, PURPLE"Shutdown display team number [NB] won\n"RESET);
		g_game_run = 0;
		sem_unlock(g_game->ipc->semid);
		destroy_windows();
	}

	for (uint32_t y = 0; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < SCREEN_WIDTH - 1; ++x) {
			uint32_t idx = ((pixel_x / TILE_SIZE) % BOARD_W) + ((pixel_y / TILE_SIZE) * BOARD_W);
			uint32_t tile_state = g_game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : tile_state % 2 ? 0x0000FF : 0xFF0000;
			if (tile_state == 10U)
				color = 0x00FF00;
			if (pixel_x % TILE_SIZE != 0 && pixel_y % TILE_SIZE != 0) {
				((uint32_t *)g_game->img.data)[x + (y * SCREEN_WIDTH)] = color;
			}
			++pixel_x;
		}
		pixel_x = 0;
		++pixel_y;
	}
	mlx_put_image_to_window(g_game->mlx, g_game->win, g_game->img.image, 0, 0);
	sem_unlock(g_game->ipc->semid);
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

	mlx_clear_window(g_game->mlx, g_game->win);
	mlx_hook(g_game->win, 2, 1L, key_hooks_press, g_game);
	mlx_hook(g_game->win, DestroyNotify, StructureNotifyMask, destroy_windows, g_game);
	// mlx_loop_hook(g_game->mlx, game_display, g_game);
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