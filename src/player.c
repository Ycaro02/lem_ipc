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
	if (id_check >= TEAM_ID_MAX) {
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

uint32_t check_double_value(uint32_t *array, uint32_t team_id)
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

uint32_t check_death(uint32_t *board, t_vec point, uint32_t team_id)
{
	t_vec	arround[DIR_MAX] = ARROUND_VEC_ARRAY(point);
	uint32_t arround_val[DIR_MAX] = {0};
	for (int i = 0; i < DIR_MAX; i++) {
		if (get_board_index(arround[i]) >= BOARD_SIZE) {
			continue;
		}
		arround_val[i] = get_tile_board_val(board, arround[i]);
	}
	return (check_double_value(arround_val, team_id));
}

/* Check if player is dead */
uint32_t check_player_death(t_ipc *ipc, t_player *player)
{
	player->kill_by = check_death(ipc->ptr, player->pos, player->team_id);
	return (player->kill_by);
}

static void put_player_on_board(t_ipc *ipc, t_player *player)
{
	t_vec	 	point;

	sem_lock(ipc->semid);

	point = get_random_point(ipc->ptr, player->pos);
	set_tile_board_val(ipc->ptr, point, player->team_id);
	player->pos = point;
	player->next_pos = point;
	/* Update info to default value */
	player->target = get_board_vec(OUT_OF_BOARD);
	player->ally_pos = get_board_vec(OUT_OF_BOARD);

 	/* Init display handler bool to know if we need to send data to display program */
	player->display = display_handler_state(ipc);

	player->state = S_WAITING;

	/* Send create packet to display */
	send_pdata_display(ipc, player, P_CREATE);
	// ft_printf_fd(1, GREEN"Player %u start at %u %u\n"RESET, player->team_id, player->pos.y, player->pos.x);
	sem_unlock(ipc->semid);
}

static int8_t check_break_loop(t_ipc *ipc, t_player *player, int8_t enemy_found)
{
	/* Check if player is dead */
	if (check_player_death(ipc, player) != ALIVE) {
		/* Send data to display */
		send_pdata_display(ipc, player, P_DELETE);
		// clear_msg_queue(ipc, player->team_id);
		g_game_run = 0;
		return (1);
	} else if (!enemy_found) { /* Check win condition */
		ft_printf_fd(1, FILL_YELLOW"End of game no enemy found team %u won\n"RESET, player->team_id);
		send_pdata_display(ipc, player, P_DELETE);
		g_game_run = 0;
		return (1);
	}
	return (0);
}

static void find_next_move(t_ipc *ipc, t_player *player, int8_t player_alone)
{
	/* Rush ally bool 1 for rush 0 for no */
	int8_t rush_ally = player_alone == 1 ? 0 : (get_heuristic_cost(player->pos, player->ally_pos) > 2);

	if (rush_ally) {
		player->next_pos = find_smarter_possible_move(ipc, player->pos, player->ally_pos, player->team_id);
		// clear_msg_queue(ipc, player->team_id);
		player->state = S_WAITING;
	} else if (!player_alone) {
		if (player->state == S_WAITING) {
			player_waiting(ipc, player);
		} else {
			player_tracker_follower(ipc, player);
		}
	} else { /* if player is alone or no enemy found*/
		// clear_msg_queue(ipc, 0);
		player->state = S_WAITING;
		player->target = get_random_point(ipc->ptr, player->pos);
		player->next_pos = find_smarter_possible_move(ipc, player->pos, player->target, player->team_id);
	}
}

void player_routine(t_ipc *ipc, t_player *player) 
{
	if (init_signal_handler() == -1) {
		return ;
	}

	/* Set First player position randomly */
	put_player_on_board(ipc, player);

	/* start routine */
	while (g_game_run) {
		sem_lock(ipc->semid);
		/* Check if display handler is active */
		player->display = display_handler_state(ipc);

		/* Player scan his environement to find nearest ally (update player->ally_pos if found) */
		int8_t player_alone = find_player_in_range(ipc, player, BOARD_W, ALLY_FLAG) == 0;
		/* Player scan his environement to find nearest enemy (update player->target if found) */
		int8_t enemy_found = find_player_in_range(ipc, player, BOARD_W, ENEMY_FLAG);

		/* Check break loop condition (death/win) */		
		if (check_break_loop(ipc, player, enemy_found))
			break ;
		
		/* Player logic AI */
		find_next_move(ipc, player, player_alone);

		/* Move */
		if (!vector_cmp(player->next_pos, player->pos)) {
			send_pdata_display(ipc, player, P_UPDATE_POS);
			
			ft_printf_fd(1, GREEN"Player "YELLOW"ID[%u] "CYAN"from [%u][%u] -> "PURPLE"[%u][%u]\n"RESET, player->team_id, player->pos.y, player->pos.x, player->next_pos.y, player->next_pos.x);
			/* Set empty last position tile */
			set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
			
			player->pos = create_vector(player->next_pos.y, player->next_pos.x);
			
			/* Set team id value in new player position */
			set_tile_board_val(ipc->ptr, player->pos, player->team_id);
		}
		sem_unlock(ipc->semid);
		usleep(PLAYER_WAIT_TIME);
		// usleep(10000);
	}
}

// if (player_alone) {
// 	ft_printf_fd(2, RED"Player %u is alone\n"RESET, player->team_id);
// } else  {
// 	ft_printf_fd(2, YELLOW"Player %u [%u][%u] is not alone, ally pos [%u][%u]\n"RESET, player->team_id, player->pos.y, player->pos.x , player->ally_pos.y, player->ally_pos.x);
// 	rush_ally = get_heuristic_cost(player->pos, player->ally_pos) > 2;
// }
// ft_printf_fd(2, YELLOW"\nPlayer %u before at %u %u: --> "RESET, player->team_id, player->pos.y, player->pos.x);
// ft_printf_fd(2, RED"Enemy found at %u %u\n"RESET, player->target.y, player->target.x);
// ft_printf_fd(2, YELLOW"Player %u after at %u %u\n"RESET, player->team_id, player->pos.y, player->pos.x);
// ft_printf_fd(2, YELLOW"Heurisctic %u after at %u %u\n"RESET, player->team_id, hp.pos.y, hp.pos.x);