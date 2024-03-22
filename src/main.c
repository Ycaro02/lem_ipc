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

	if (init_player(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, PLAYER) == ERROR_CASE) {
		return (1);
	}

	player_routine(&ipc, &player);
	ft_printf_fd(2, YELLOW"Lem-Ipc Client team nb [%d] die on [%d][%d]\nTeam nb bool = |%d| nb att = |%d|\n"RESET\
		, player.team_id, player.pos.y, player.pos.x, find_player_in_range(&ipc, &player, (int)BOARD_W, ALLY_FLAG), get_attached_processnb(&ipc));
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);

	if (get_attached_processnb(&ipc) == 1) {
		clean_shared_rsc(&ipc);
		ft_printf_fd(1, RED"Lem-ipc Server Down Team %u Won\n"RESET, player.team_id);
	} else {
		detach_shared_memory(&ipc);
		sem_unlock(ipc.semid);
	}
	return (ret);
}