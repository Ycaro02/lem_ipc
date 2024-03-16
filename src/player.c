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

/* check 8 point arround  */
uint8_t check_arround_point(uint32_t *board, t_vec point, uint32_t team_id)
{
	t_vec	 	arround[ARROUND_VEC_SIZE] = ARROUND_VEC_ARRAY(point);
	uint8_t		enemy_arround = 0;
	uint32_t	ret = 0;

	for (int i = 0; i < ARROUND_VEC_SIZE; i++) {
		if (get_board_index(arround[i]) >= BOARD_SIZE) {
			continue;
		}
		ret = get_tile_board_val(board, arround[i]);
		if (ret != team_id && ret != TILE_EMPTY) { /* basic implementation need to store value for each enemy team and leave if 2 is reached */
			++enemy_arround;
		}
	}
	ret = (enemy_arround >= 2); /* if dead return 1 otherwise 0 */
	return (ret);
}

/* Check if player is dead */
uint8_t check_player_death(t_ipc *ipc, t_player *player)
{
	uint8_t		ret = 0;
	
	ret = check_arround_point(ipc->ptr, player->pos, player->team_id);
	return (ret);
}

void player_routine(t_ipc *ipc, t_player *player) 
{
	// uint32_t	to_rush;
	t_vec	 	point;

	if (init_signal_handler() == -1) {
		return ;
	}
	/* Set First player position randomly */
	sem_lock(ipc->semid);
	team_handling(ipc->ptr, player->team_id, ADD_TEAM);

	point = get_random_point(ipc->ptr, player->pos);
	ft_printf_fd(2, "Player %u start at %u %u\n", player->team_id, point.x, point.y);
	set_tile_board_val(ipc->ptr, point, player->team_id);
	player->pos = point;
	sem_unlock(ipc->semid);
	/* start routine */
	while (g_game_run) {
		sem_lock(ipc->semid);
		// if (check_player_death(ipc, player) || ipc->ptr[TEAM_NB] == 1) {
		// 	// char *color = player->team_id % 2  ? RED : BLUE;
		// 	set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
		// 	g_game_run = 0;
		// 	sem_unlock(ipc->semid);			
		// 	break;
		// }

		// // to_rush =  extract_msg(ipc, player);
		// // send_msg(ipc, player, to_rush);

		// point = get_random_point(ipc->ptr, player->pos);
		// if (!vector_cmp(point, player->pos)) {
		// 	/* Set empty last position tile */
		// 	set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
		// 	player->pos = point;
		// 	/* Set team id value in new player position */
		// 	set_tile_board_val(ipc->ptr, point, player->team_id);
		// }
		sem_unlock(ipc->semid);
		usleep(100000); /* 1/10 sec */
	}
}


// static t_vec find_smarter_possible_move(t_vec current, t_vec end)
// {
// 	t_vec possible_move[] = ARROUND_VEC_ARRAY(current);
// 	t_vec arround_end[] = ARROUND_VEC_ARRAY(end);


// }

// static t_vec find_enemy_inrange(t_ipc *ipc, t_vec pos, uint32_t team_id, uint32_t range)
// {
// 	uint32_t	tile_state = TILE_EMPTY;
// 	t_vec		arround[ARROUND_VEC_SIZE] = ARROUND_VECX_ARRAY(pos, range);


// 	for (int i = 0; i < ARROUND_VEC_SIZE; i++) {
// 		if (get_board_index(arround[i]) >= BOARD_SIZE) {
// 			continue;
// 		}
// 		tile_state = get_tile_board_val(ipc->ptr, arround[i]);
// 		if (tile_state != team_id && tile_state != TILE_EMPTY) {
// 			return (arround[i]);
// 		}
// 	}
// 	return (pos);
// }

// static int8_t scan_board_arround(t_ipc *ipc, t_player *player, uint32_t range_max)
// {
// 	uint32_t	tile_state = TILE_EMPTY;

// 	for (uint32_t range = 1; range <= range_max; range++) {
// 		player->target = find_enemy_inrange(ipc, player->pos, player->team_id, range);
// 		if (!vector_cmp(player->target, player->pos)) {
// 			return (1);
// 		}
// 	}
// 	return (0);
// }

/*

enum e_player_state {
	S_WAITING,
	S_TRACKER,
	S_FOLLOWER,
};

WAITING logic
{
	1: Player try to receive message:
	- If message received, store value (enemy pos + first player heuristic)
	2: Player will scan his environement X tile arround to find the best enemy to rush
	- If no enemy found and no message receive
		- stay in WAITING state
		- move to a random position
		- don't send message
	- If enemy found and message receive :
		- compare the heuristic of the enemy found with the heuristic of enemy position received
	- If heuristic of the enemy is better, or no message receive:
		- become a TRACKER
		- move nearby to the enemy position
		- send message to his team (enemy pos, new heuristic)
	- Else if heuristic of the enemy is worst thans heuristic compute and heuristic receive is arround the heuristique compute
		- become a FOLLOWER
		- move to the position of the message received
		- don't send message to his team (maybe just send a message to say he is following, with his heuristic cost to reach the enemy)
	- ELse (msg receive case, if not we are in the first if case)
		- stay in WAITING state
		- move to position of the message received
		- resend the receive msg to the team
}

FOLLOWER logic 
{
	1: Ignore message
	2: Scan env if no enemy in X tile arround break follower logic and go WAITING case (target probably dead)
	Else
		- move to the position of the nearest enemy (supposed to be the position of enemy targeted by the last message received)
		- don't send message
		- stay a FOLLOWER
}

TRACKER logic
{
	1: Ignore message
	2: Scan env if no enemy in X tile arround break tracker logic and go WAITING case (target probably dead)
	Else
		- compute new heuristic to reach the enemy
		- move to the position of this enemy
		- send message to his team (enemy pos, new heuristic) ?(maybe don't sned message)
		- stay a TRACKER
}


*/