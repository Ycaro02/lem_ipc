# include "../include/lem_ipc.h"


int g_game_run;

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


void signal_handler(int signum)
{
	g_game_run = 0;
	(void)signum;
	ft_printf_fd(2, RED"\nSIGINT Catch in display: %d\n"RESET, signum);
	return ;
}

int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		ft_printf_fd(2, "Can't catch SIGINT\n");
		return (-1);
	}
	return (0);
}

void game_loop(t_ipc *ipc, uint32_t id) {
	init_signal_handler();
	(void)ipc;
	(void)id;
	while (g_game_run) {
		semaphore_lock(ipc->semid);
		display_uint16_array(ipc->ptr);
		if (get_attached_processnb(ipc) <= 2) {
			ft_printf_fd(2, RED"Shutdown display\n"RESET);
			g_game_run = 0;
		}
		semaphore_unlock(ipc->semid);
		sleep(1);
	}
}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};

	g_game_run = 1;
	// ft_printf_fd(1, "Lem-ipc start %d\n", g_game_run);

	if (init_display(&player, argc, argv) != 0) {
		return (1);
	}

	if (init_semaphores_set(&ipc, IPC_NAME, DISPLAY_HANDLER) == -1) {
		return (1);
	}
	
	// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);

	game_loop(&ipc, player.team_id);

	semaphore_lock(ipc.semid);
	ft_printf_fd(2, RED"Lem-ipc number %d end\n"RESET, player.team_id);	
	semaphore_unlock(ipc.semid);

	return (0);
}