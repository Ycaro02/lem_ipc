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
	t_vec	 	arround[DIR_MAX] = ARROUND_VEC_ARRAY(point);
	uint8_t		enemy_arround = 0;
	uint32_t	ret = 0;

	for (int i = 0; i < DIR_MAX; i++) {
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
	t_heuristic hp;

	if (init_signal_handler() == -1) {
		return ;
	}
	/* Set First player position randomly */
	sem_lock(ipc->semid);
	team_handling(ipc->ptr, player->team_id, ADD_TEAM);

	point = get_random_point(ipc->ptr, player->pos);
	ft_printf_fd(2, RED"Player %u start at %u %u\n"RESET, player->team_id, point.y, point.x);
	set_tile_board_val(ipc->ptr, point, player->team_id);
	player->pos = point;
	sem_unlock(ipc->semid);
	/* start routine */
	while (g_game_run) {
		sem_lock(ipc->semid);

		ft_printf_fd(2, YELLOW"\nPlayer %u before at %u %u: --> "RESET, player->team_id, player->pos.y, player->pos.x);
		if (find_enemy_inXrange(ipc, player, 20)){
			ft_printf_fd(2, RED"Enemy found at %u %u\n"RESET, player->target.y, player->target.x);
		}
		hp = find_smarter_possible_move(ipc, player->pos, player->target);

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
		if (!vector_cmp(hp.pos, player->pos)) {
			/* Set empty last position tile */
			set_tile_board_val(ipc->ptr, player->pos, TILE_EMPTY);
			player->pos = create_vector(hp.pos.y, hp.pos.x);
			// ft_printf_fd(2, CYAN"Player %u move to %u %u\n\n\n"RESET, player->team_id, player->pos.y, player->pos.x);
			/* Set team id value in new player position */
			set_tile_board_val(ipc->ptr, player->pos, player->team_id);
		}
		sem_unlock(ipc->semid);
		sleep(2);
		// usleep(100000); /* 1/10 sec */
	}
}

// ft_printf_fd(2, YELLOW"Player %u after at %u %u\n"RESET, player->team_id, player->pos.y, player->pos.x);
// ft_printf_fd(2, YELLOW"Heurisctic %u after at %u %u\n"RESET, player->team_id, hp.pos.y, hp.pos.x);