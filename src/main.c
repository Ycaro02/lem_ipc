# include "../include/lem_ipc.h"

/* Game run status, global to handle sigint */
int g_game_run;

/**
 * Display player end, set the player tile to empty
 * @param ipc IPC structure
 * @param player Player structure
 * @return The number of process attached to the shared memory
 */
static int display_player_end(IPC ipc, Player player)
{
	int		nb_process = get_attached_processnb(&ipc);
	set_tile_board_val(ipc.ptr, player.pos, TILE_EMPTY);
	return (nb_process);
}

/**
 * Wait for display handler to finish
 * @param ipc IPC structure
 * @return The number of process attached to the shared memory
 */
static int wait_display_handler_finish(IPC *ipc)
{
	int nb_process = 2;
	sem_unlock(ipc->semid);
	sleep(1);
	sem_lock(ipc->semid);
	nb_process = get_attached_processnb(ipc);
	return (nb_process);
}

int main(int argc, char **argv) 
{
	IPC		ipc;
	Player	player;
	int		ret = 0, nb_process = 0;
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
		while (nb_process != 1) {
			nb_process = wait_display_handler_finish(&ipc);
		}
	}

	if (nb_process == 1) {
		clean_shared_rsc(&ipc);
		ft_printf_fd(1, FILL_YELLOW"\nLem-ipc Server Down Team |%u| Won\n"RESET, player.team_id);
	} else {
		detach_shared_memory(&ipc);
		sem_unlock(ipc.semid);
	}
	return (ret);
}

