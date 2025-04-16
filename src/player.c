# include "../include/lem_ipc.h"

/* @brief Initialize player */
int init_player(Player *player, int argc, char **argv)
{
	u64 id_check;

	if (argc != 2) {
		ft_printf_fd(2, "Usage: %s <TEAM_ID>\n", argv[0]);
		return (-1);
	}
	id_check = array_to_uint32(argv[1]);
	if (id_check >= TEAM_ID_MAX) {
		ft_printf_fd(2, "Invalid team id\n");
		return (-1);
	}
	player->team_id = (u32)id_check;
	return (0);
}

static void signal_handler(int signum)
{
	g_game_run = 0;
	(void)signum;
	return ;
}

int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		return (-1);
	}
	return (0);
}

u32 check_double_value(u32 *array, u32 team_id)
{
	for (int i = 0; i < DIR_MAX; i++) {
		for (int j = 0; j < DIR_MAX; j++) {
			if (i != j && array[i] == array[j]\
				&& array[i] != TILE_EMPTY && array[i] != team_id) {
				return (array[i]);
			}
		}
	}
	return (ALIVE);
}

u32 check_death(u32 *board, t_vec point, u32 team_id)
{
	t_vec	arround[DIR_MAX] = ARROUND_VEC_ARRAY(point);
	u32 arround_val[DIR_MAX] = {0};
	for (int i = 0; i < DIR_MAX; i++) {
		if (get_board_index(arround[i]) >= BOARD_SIZE) {
			continue;
		}
		arround_val[i] = get_tile_board_val(board, arround[i]);
	}
	return (check_double_value(arround_val, team_id));
}

/* Check if player is dead */
u32 check_player_death(IPC *ipc, Player *player)
{
	player->kill_by = check_death(ipc->ptr, player->pos, player->team_id);
	return (player->kill_by);
}

static int put_player_on_board(IPC *ipc, Player *player)
{
	t_vec	 	point;

	sem_lock(ipc->semid);

	point = get_random_point(ipc->ptr);

	if (point.x == UINT32_MAX && point.y == UINT32_MAX) {
		ft_printf_fd(1, "No more tile available\n");
		sem_unlock(ipc->semid);
		return (ERROR_CASE) ;
	}

	set_tile_board_val(ipc->ptr, point, player->team_id);
	player->pos = point;
	player->next_pos = point;
	/* Update info to default value */
	player->target = get_board_vec(OUT_OF_BOARD);
	player->ally_pos = get_board_vec(OUT_OF_BOARD);

 	/* Init display handler bool to know if we need to send data to display program */
	player->state = S_WAITING;

	wait_for_display_handler_connect(ipc);

	/* Send create packet to display */
	send_pdata_display(ipc, player, P_CREATE);
	sem_unlock(ipc->semid);
	return (TRUE);
}

static s8 check_break_loop(IPC *ipc, Player *player, s8 enemy_found)
{
	/* Check if player is dead */
	if (check_player_death(ipc, player) != ALIVE) {
		/* Send data to display */
		send_pdata_display(ipc, player, P_DELETE);
		g_game_run = 0;
		return (1);
	} else if (!enemy_found) { /* Check win condition */
		send_pdata_display(ipc, player, P_DELETE);
		g_game_run = 0;
		return (1);
	}
	return (0);
}

static void find_next_move(IPC *ipc, Player *player, s8 player_alone)
{
	/* Rush ally bool 1 for rush 0 for no */
	s8 rush_ally = player_alone == 1 ? 0 : (get_heuristic_cost(player->pos, player->ally_pos) > 2);

	if (rush_ally) {
		player->next_pos = find_smarter_possible_move(ipc, player->pos, player->ally_pos);
		player->state = S_WAITING;
	} else if (!player_alone) {
		if (player->state == S_WAITING) {
			player_waiting(ipc, player);
		} else {
			player_tracker_follower(ipc, player);
		}
	} else { /* if player is alone or no enemy found*/
		player->state = S_WAITING;
		player->target = get_random_point(ipc->ptr);
		player->next_pos = find_smarter_possible_move(ipc, player->pos, player->target);
	}
}

int player_routine(IPC *ipc, Player *player) 
{
	// if (init_signal_handler() == -1) { return (ERROR_CASE);	}

	/* Set First player position randomly */
	if (put_player_on_board(ipc, player) == ERROR_CASE) { return (ERROR_CASE); }

	/* start routine */
	while (g_game_run) {
		sem_lock(ipc->semid);
		/* Check if display handler is active */
		ipc->display = display_handler_state(ipc);

		u32 playing_state = get_playing_state(ipc->ptr);

		if (ipc->display == DH_PRIORITY || playing_state == FALSE) {
			sem_unlock(ipc->semid);
			usleep(100000);
			continue; /* wait for display handler to finish process message */
		}

		/* Player scan his environement to find nearest ally (update player->ally_pos if found) */
		s8 player_alone = find_player_in_range(ipc, player, BOARD_W, ALLY_FLAG) == 0;
		/* Player scan his environement to find nearest enemy (update player->target if found) */
		s8 enemy_found = find_player_in_range(ipc, player, BOARD_W, ENEMY_FLAG);

		/* Check break loop condition (death/win) */		
		if (check_break_loop(ipc, player, enemy_found))
			break ;
		
		/* Player logic AI */
		find_next_move(ipc, player, player_alone);

		/* Move */
		if (!vector_cmp(player->next_pos, player->pos)) {
			send_pdata_display(ipc, player, P_UPDATE_POS);
			
			/* Set empty last position tile */
			set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
			
			player->pos = create_vector(player->next_pos.y, player->next_pos.x);
			
			/* Set team id value in new player position */
			set_tile_board_val(ipc->ptr, player->pos, player->team_id);
		}
		sem_unlock(ipc->semid);
		usleep(PLAYER_WAIT_TIME);
	}
	return (TRUE);
}