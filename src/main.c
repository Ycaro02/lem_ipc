# include "../include/lem_ipc.h"

int g_game_run;


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

	player_routine(&ipc, &player);


	sem_lock(ipc.semid);
	ft_printf_fd(2, YELLOW"Lem-Ipc Client team nb [%d] die on [%d][%d]\n"RESET, player.team_id, player.pos.y, player.pos.x);
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	team_handling(ipc.ptr, player.team_id, RM_TEAM);

	if (get_attached_processnb(&ipc) == 1) {
		// clean_shared_rsc(&ipc);
		ft_printf_fd(1, RED"Lem-ipc Server Down Team %u Won\n"RESET, player.team_id);
	} else {
		sem_unlock(ipc.semid);
	}
	return (ret);
}

// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
// uint32_t val = player.team_id;
// sem_lock(ipc.semid);
// t_vec point = get_random_point(ipc.ptr);
// ft_printf_fd(2, YELLOW"Lem-ipc Client team number %u start pos [%u][%u]\n"RESET, player.team_id, point.x, point.y);
// set_tile_board_val(ipc.ptr, point, val);
// sem_unlock(ipc.semid);