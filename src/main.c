# include "../include/lem_ipc.h"

/* Can be remove game run sattus */
int g_game_run;

static int display_player_end(IPC ipc, Player player)
{
	int		nb_process = get_attached_processnb(&ipc);
	// s8	lasTeam = find_player_in_range(&ipc, &player, (int)BOARD_W, ENEMY_FLAG);
	// char	*team = lasTeam == 0 ? FILL_GREEN"Last Team"RESET : FILL_RED"Not Last Team"RESET;
	// char	*str_status = lasTeam == 0 ? "Won" : "Lost";
	// char	*color = lasTeam == 0 ? FILL_GREEN : FILL_YELLOW;

	// ft_printf_fd(2, "%sLem-IPC Client team nb [%d] %s on [%d][%d]\n"RESET"%s\n"FILL_YELLOW"Attached = |%d|\n"RESET, color, player.team_id, str_status, player.pos.y, player.pos.x, team, nb_process);
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	return (nb_process);
}

static int wait_display_handler_finish(IPC *ipc)
{
	int nb_process = 2;
	// ft_printf_fd(1, FILL_RED"Lem-ipc Server Want Down wait display handler nb process: %d\n"RESET, nb_process);
	sem_unlock(ipc->semid);
	sleep(5);
	sem_lock(ipc->semid);
	nb_process = get_attached_processnb(ipc);
	// ft_printf_fd(1, FILL_RED"Lem-ipc Server Want Down after sleep display handler nb process: %d\n"RESET, nb_process);
	return (nb_process);
}

int main(int argc, char **argv) 
{
	IPC		ipc;
	Player	player;
	int			ret = 0, nb_process = 0;
	s8		display_handler = 0;


	ft_bzero(&ipc, sizeof(IPC));
	ft_bzero(&player, sizeof(Player));
	g_game_run = 1;

	if (init_player(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, PLAYER) == ERROR_CASE) {
		return (1);
	}
	if (player_routine(&ipc, &player) == ERROR_CASE) {
		detach_shared_memory(&ipc);
		return (1);
	}
	nb_process = display_player_end(ipc, player);
	display_handler = display_handler_state(&ipc);
	if (nb_process == 2 && display_handler == 1) {
		nb_process = wait_display_handler_finish(&ipc);
	}
	if (nb_process == 1) {
		clean_shared_rsc(&ipc);
		ft_printf_fd(1, FILL_YELLOW"Lem-ipc Server Down Team |%u| Won\n"RESET, player.team_id);
	} else {
		detach_shared_memory(&ipc);
		// ft_printf_fd(1, YELLOW"Lem-ipc Client Down NB remain %d\n"RESET, nb_process);
		sem_unlock(ipc.semid);
	}
	return (ret);
}

