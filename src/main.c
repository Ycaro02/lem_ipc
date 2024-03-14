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
	id_check = array_to_uint32(argv[1]);
	if (id_check == OUT_OF_UINT32) {
		ft_printf_fd(2, "Invalid team id\n");
		return (-1);
	}
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


void game_loop(t_ipc *ipc, t_player *player) {
	init_signal_handler();
	while (g_game_run) {
		sem_lock(ipc->semid);
		t_vec point = get_reachable_point(ipc->ptr);
		set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
		player->pos = point;
		// ft_printf_fd(2, YELLOW"Lem-ipc Client team number %u goto pos [%u][%u]\n"RESET, id, point.x, point.y);
		set_tile_board_val(ipc->ptr, point, player->team_id);
		sem_unlock(ipc->semid);
		usleep(100000); /* 1/10 sec */
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

	game_loop(&ipc, &player);


	sem_lock(ipc.semid);
	ft_printf_fd(2, YELLOW"Lem-ipc Client team number %d die restore [%d][%d]\n"RESET, player.team_id, player.pos.y, player.pos.x);
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	sem_unlock(ipc.semid);

	if (get_attached_processnb(&ipc) == 1) { /* need to check here for victory instead */
		ft_printf_fd(1, RED"Lem-ipc Server Down %d %d\n"RESET, g_game_run, player.team_id);
		clean_shared_memory(&ipc);
	}
	return (ret);
}

// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
// uint32_t val = player.team_id;
// sem_lock(ipc.semid);
// t_vec point = get_reachable_point(ipc.ptr);
// ft_printf_fd(2, YELLOW"Lem-ipc Client team number %u start pos [%u][%u]\n"RESET, player.team_id, point.x, point.y);
// set_tile_board_val(ipc.ptr, point, val);
// sem_unlock(ipc.semid);