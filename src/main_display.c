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
	sleep(1);
	return (0);
}

/* to put in header file */


int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game);
	return (0);
}


void init_mlx() {
	g_game->mlx = mlx_init();
	g_game->win = mlx_new_window(g_game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	mlx_hook(g_game->win, 2, 1L, key_hooks_press, g_game);
	mlx_hook(g_game->win, DestroyNotify, StructureNotifyMask, destroy_windows, g_game);
	mlx_loop_hook(g_game->mlx, game_display, g_game);
	mlx_loop(g_game->mlx);

}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	
	g_game = malloc(sizeof(t_game));

	g_game->ipc = &ipc;

	if (init_display(&player, argc, argv) != 0) {
		return (1);
	}

	if (init_game(&ipc, IPC_NAME, DISPLAY_HANDLER) == ERROR_CASE) {
		return (1);
	}
	
	init_mlx();
	// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);

	sem_lock(ipc.semid);
	ft_printf_fd(2, RED"Lem-ipc Display Handler end\n"RESET, player.team_id);	
	sem_unlock(ipc.semid);

	return (0);
}