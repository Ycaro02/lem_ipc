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
	if (id_check >= UINT32_MAX) {
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

int8_t check_double_value(uint32_t *array, uint32_t team_id)
{
	for (int i = 0; i < DIR_MAX; i++) {
		for (int j = 0; j < DIR_MAX; j++) {
			if (i != j && array[i] == array[j]\
				&& array[i] != TILE_EMPTY && array[i] != team_id) {
				return (1);
			}
		}
	}
	return (0);
}

int8_t check_death(uint32_t *board, t_vec point, uint32_t team_id)
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
uint8_t check_player_death(t_ipc *ipc, t_player *player)
{
	uint8_t		ret = 0;
	
	ret = check_death(ipc->ptr, player->pos, player->team_id);
	return (ret);
}

static void put_player_on_board(t_ipc *ipc, t_player *player)
{
	t_vec	 	point;

	sem_lock(ipc->semid);
	team_handling(ipc->ptr, player->team_id, ADD_TEAM);
	point = get_random_point(ipc->ptr, player->pos);
	ft_printf_fd(2, CYAN"Player in team |%u| start at [%u] [%u]\n"RESET, player->team_id, point.y, point.x);
	set_tile_board_val(ipc->ptr, point, player->team_id);
	player->pos = point;
	player->target = point;
	sem_unlock(ipc->semid);
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

		if (check_player_death(ipc, player)) {
			set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
			clear_msg_queue(ipc, player->team_id);
			g_game_run = 0;
			sem_unlock(ipc->semid);			
			break;
		} else if (ipc->ptr[TEAM_NB] <= 1) {
			g_game_run = 0;
			sem_unlock(ipc->semid);			
			break;
		}
		
		int8_t player_alone = (find_player_in_range(ipc, player, (int)BOARD_W, ALLY_FLAG) == 0);
		// if (player_alone) {
		// 	ft_printf_fd(2, YELLOW"Player %u is alone\n"RESET, player->team_id);
		// }

		/* Player scan his environement to find nearest enemy */
		if (!player_alone && find_player_in_range(ipc, player, (int)BOARD_W, ENEMY_FLAG) == 1) {
			player->next_pos = find_smarter_possible_move(ipc, player->pos, player->target, player->team_id);
			if (player->state == S_WAITING) {
				player_waiting(ipc, player);
			} else {
				player_tracker_follower(ipc, player);
			}
		} else {
			// ft_printf_fd(1, GREEN"\nPlayer %u no enemy/ally found clear msg_Q go waiting random point\n\n"RESET, player->team_id);
			// find_player_in_range(ipc, player, (int)BOARD_W, ENEMY_FLAG);
			clear_msg_queue(ipc, player->team_id);
			player->state = S_WAITING;
			player->target = get_random_point(ipc->ptr, player->pos);
			player->next_pos = find_smarter_possible_move(ipc, player->pos, player->target, player->team_id);
		}
		
		if (!vector_cmp(player->next_pos, player->pos)) {
			/* Set empty last position tile */
			set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
			player->pos = create_vector(player->next_pos.y, player->next_pos.x);
			// ft_printf_fd(2, CYAN"Player %u move to %u %u\n\n\n"RESET, player->team_id, player->pos.y, player->pos.x);
			/* Set team id value in new player position */
			set_tile_board_val(ipc->ptr, player->pos, player->team_id);
		}
		sem_unlock(ipc->semid);
		// sleep(2);
		usleep(500000); /* 1/2 sec */
		// usleep(100000); /* 1/10 sec */
		// usleep(500);
	}
}

// ft_printf_fd(2, YELLOW"\nPlayer %u before at %u %u: --> "RESET, player->team_id, player->pos.y, player->pos.x);
// ft_printf_fd(2, RED"Enemy found at %u %u\n"RESET, player->target.y, player->target.x);
// ft_printf_fd(2, YELLOW"Player %u after at %u %u\n"RESET, player->team_id, player->pos.y, player->pos.x);
// ft_printf_fd(2, YELLOW"Heurisctic %u after at %u %u\n"RESET, player->team_id, hp.pos.y, hp.pos.x);