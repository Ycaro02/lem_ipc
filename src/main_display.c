# include "../include/lem_ipc.h"

int g_game_run; /* unused in displayer */
/* GARBAGE HERE */

# define TILE_SIZE 64

# define SCREEN_WIDTH TILE_SIZE * BOARD_W
# define SCREEN_HEIGHT TILE_SIZE * BOARD_H

t_game *g_game;

typedef enum e_key
{
	ESC = 65307,
	UP = 119,
	LEFT = 97,
	DOWN = 115,
	RIGHT = 100,
	ARROW_UP = 65362,
	ARROW_LEFT = 65361,
	ARROW_RIGHT = 65363,
	ARROW_DOWN = 65364,
}	t_key;


/* @brief Initialize player */

int init_display(t_player *player, int argc, char **argv)
{
	if (argc != 1) {
		ft_printf_fd(2, "Usage: no args required %s\n", argv[0]);
		return (-1);
	}
	player->team_id = UINT32_MAX;
	return (0);
}

void signal_handler(int signum) {
	g_game_run = 0;
	(void)signum;
}

int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		ft_printf_fd(2, "Can't catch SIGINT\n");
		return (-1);
	}
	return (0);
}

int game_display() {
	sem_lock(g_game->ipc->semid);
	display_uint16_array(g_game->ipc->ptr);
	if (get_attached_processnb(g_game->ipc) <= 1) {
		ft_printf_fd(2, RED"Shutdown display\n"RESET);
		g_game_run = 0;
	}
	sem_unlock(g_game->ipc->semid);
	sleep(1);
	return (0);
}

/* to put in header file */

int destroy_windows()
{
	mlx_destroy_window(g_game->mlx, g_game->win);
	mlx_destroy_display(g_game->mlx);
	free(g_game->mlx);
	free(g_game);
	ft_printf_fd(1, "Exit MLX\n");
	exit(0);
}

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