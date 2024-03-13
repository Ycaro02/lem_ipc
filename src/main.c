# include "../include/lem_ipc.h"


int g_game_run;

/* @brief Initialize player */

int init_player(t_player *player, int argc, char **argv)
{
	uint64_t id_check;

	if (argc != 2) {
		ft_printf_fd(2, "Usage: %s <TEAM_ID>\n", argv[0]);
		return (-1);
	}
	// player->pos = create_vector(0, 0);
	// player->target = create_vector(0, 0);
	id_check = array_to_uint32(argv[1]);
	if (id_check == OUT_OF_UINT32) {
		ft_printf_fd(2, "Invalid team id\n");
		return (-1);
	}
	// ft_printf_fd(2, YELLOW"New player Join team number:%s %s[%u]%s\n", RESET, RED, (uint32_t)id_check, RESET);
	player->team_id = (uint32_t)id_check;
	return (0);
}


void signal_handler(int signum)
{
	g_game_run = 0;
	(void)signum;
	// ft_printf_fd(2, RED"\nSIGINT Catch: %d\n"RESET, signum);
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
		// semaphore_lock(ipc->semid);
		// ft_printf_fd(1, "Lem-ipc number %d run\n",id);
		// semaphore_unlock(ipc->semid);
		sleep(1);
	}
}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	int			ret = 0;

	g_game_run = 1;
	// ft_printf_fd(1, "Lem-ipc start %d\n", g_game_run);

	if (init_player(&player, argc, argv) != 0) {
		return (1);
	}

	if (init_game(&ipc, IPC_NAME, PLAYER) == ERROR_CASE) {
		return (1);
	}
	
	// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
	uint32_t val = player.team_id;

	semaphore_lock(ipc.semid);
	set_tile_board_val(ipc.ptr, create_vector(0, val), val);
	semaphore_unlock(ipc.semid);

	game_loop(&ipc, player.team_id);

	semaphore_lock(ipc.semid);
	ft_printf_fd(2, RED"Lem-ipc number %d end\n"RESET, player.team_id);	
	semaphore_unlock(ipc.semid);


	if (get_attached_processnb(&ipc) == 1) {
		display_uint16_array(ipc.ptr);
		ft_printf_fd(1, CYAN"Lem-ipc end %d %d\n"RESET, g_game_run, player.team_id);
		// semctl(ipc.semid, 0, GETVAL) == 0 ? semaphore_unlock(ipc.semid) : ft_printf_fd(2, "Nothing todo\n");
		clean_shared_memory(&ipc);
	}
	return (ret);
}