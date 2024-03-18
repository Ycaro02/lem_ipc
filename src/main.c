# include "../include/lem_ipc.h"

int g_game_run;


int main(int argc, char **argv) 
{
	t_ipc		ipc;
	t_player	player;
	int			ret = 0;


	ft_bzero(&ipc, sizeof(t_ipc));
	ft_bzero(&player, sizeof(t_player));
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
	ft_printf_fd(2, YELLOW"Lem-Ipc Client team nb [%d] die on [%d][%d]\nTeam nb = |%u| nb att = |%d|\n"RESET\
		, player.team_id, player.pos.y, player.pos.x, ipc.ptr[TEAM_NB], get_attached_processnb(&ipc));
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	team_handling(ipc.ptr, player.team_id, RM_TEAM);

	if (ipc.ptr[TEAM_NB] == 0) {
		ft_printf_fd(1, PURPLE"Lem-ipc Server Want Down Wait Display Server ...\n"RESET, player.team_id);
		sem_unlock(ipc.semid);
		sleep(1); /* just wait display server */
		sem_lock(ipc.semid);
	}

	if (get_attached_processnb(&ipc) == 1) {
		clean_shared_rsc(&ipc);
		ft_printf_fd(1, RED"Lem-ipc Server Down Team %u Won\n"RESET, player.team_id);
	} else {
		detach_shared_memory(&ipc);
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