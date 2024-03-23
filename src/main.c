# include "../include/lem_ipc.h"

int g_game_run;


static int display_player_end(t_ipc ipc, t_player player)
{
	int		nb_process = get_attached_processnb(&ipc);
	// int8_t	last_team = find_player_in_range(&ipc, &player, (int)BOARD_W, ENEMY_FLAG);
	// char	*team = last_team == 0 ? FILL_GREEN"Last Team"RESET : FILL_RED"Not Last Team"RESET;
	// char	*str_status = last_team == 0 ? "Won" : "Lost";
	// char	*color = last_team == 0 ? FILL_GREEN : FILL_YELLOW;

	// ft_printf_fd(2, "%sLem-Ipc Client team nb [%d] %s on [%d][%d]\n"RESET"%s\n"FILL_YELLOW"Attached = |%d|\n"RESET, color, player.team_id, str_status, player.pos.y, player.pos.x, team, nb_process);
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	return (nb_process);
}

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

	int nb_process = display_player_end(ipc, player);
	
	// if (nb_process == 2) {
	// 	ft_printf_fd(1, PURPLE"Lem-ipc Server Want Down wait display handler nb process: %d\n"RESET, nb_process);
	// 	sem_unlock(ipc.semid);
	// 	sleep(5);
	// 	sem_lock(ipc.semid);
	// 	nb_process = get_attached_processnb(&ipc);
	// 	ft_printf_fd(1, CYAN"Lem-ipc Server Want Down after sleep display handler nb process: %d\n"RESET, nb_process);
	// }
	if (nb_process == 1) {
		clean_shared_rsc(&ipc);
		ft_printf_fd(1, RED"Lem-ipc Server Down Team %u Won\n"RESET, player.team_id);
	} else {
		detach_shared_memory(&ipc);
		ft_printf_fd(1, YELLOW"Lem-ipc Client Down NB remain %d Won\n"RESET, nb_process);
		sem_unlock(ipc.semid);
	}
	return (ret);
}