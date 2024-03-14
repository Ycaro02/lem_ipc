# include "../include/lem_ipc.h"

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

static void signal_handler(int signum)
{
	g_game_run = 0;
	(void)signum;
	// ft_printf_fd(2, RED"\nSIGINT Catch: %d\n"RESET, signum);
	return ;
}

static int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		// ft_printf_fd(2, "Can't catch SIGINT\n");
		return (-1);
	}
	return (0);
}

void player_routine(t_ipc *ipc, t_player *player) 
{
	uint32_t	to_rush;
	t_vec	 	point;

	if (init_signal_handler() == -1) {
		return ;
	}
	while (g_game_run) {
		sem_lock(ipc->semid);
		to_rush =  extract_msg(ipc, player);
		send_msg(ipc, player, to_rush);
		point = get_reachable_point(ipc->ptr);
		/* Set empty last position tile */
		set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
		player->pos = point;
		/* Set team id value in new player position */
		set_tile_board_val(ipc->ptr, point, player->team_id);
		sem_unlock(ipc->semid);
		usleep(100000); /* 1/10 sec */
	}
}
