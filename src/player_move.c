#include "../include/lem_ipc.h"

/**
 * @brief Get the heuristic cost
 * @param start The start position
 * @param end The end position
 * @return The heuristic cost
*/
uint32_t get_heuristic_cost(t_vec start, t_vec end)
{
	uint32_t cost = 0;

	cost = abs_diff(start.x, end.x) + abs_diff(start.y, end.y);
	// ft_printf_fd(2, YELLOW"cost for %u %u to %u %u = %u\n"RESET, start.x, start.y, end.x, end.y, cost);
	return (cost);
}

/**
 * @brief Find the best possible move
 * @param ipc The ipc structure
 * @param current The current position
 * @param end The end position
 * @return The best heuristic
*/
t_heuristic find_smarter_possible_move(t_ipc *ipc, t_vec current, t_vec end)
{
	t_vec possible_move[DIR_MAX] = ARROUND_VEC_ARRAY(current);
	t_vec enemy[DIR_MAX] = ARROUND_VEC_ARRAY(end);

    t_heuristic best_heuristic = {UINT32_MAX, create_vector(current.y, current.x)};
    uint32_t	test = UINT32_MAX;

    /* loop on player possible move */
	for (int i = 0; i < DIR_MAX; i++) {
        /* if tile empty check heuristic for all end case*/
        if (possible_move[i].x >= (BOARD_W) || possible_move[i].y >= BOARD_H) { /* uglys*/
            continue;
        } else if (get_board_index(possible_move[i]) >= BOARD_SIZE) {
			continue;
		}

		if (get_tile_board_val(ipc->ptr, possible_move[i]) == TILE_EMPTY) {
			for (int j = 0; j < DIR_MAX; j++) {
                test = get_heuristic_cost(possible_move[i], enemy[j]);            
            	if (test < best_heuristic.cost) {
                    best_heuristic.cost = test;
                    best_heuristic.pos = possible_move[i];
                }
			}
		}
	}
	return (best_heuristic);
}

/**
 * @brief Check if the position is correct
 * @param ipc The ipc structure
 * @param player The player structure
 * @param x The x position
 * @param y The y position
 * @return 1 and set player->targer if enemy found, otherwise return 0
*/

int8_t is_enemy_tile(t_ipc *ipc, t_player *player, uint32_t x, uint32_t y)
{
	uint32_t	tile_state = TILE_EMPTY;
	if (x >= (BOARD_W) || y >= BOARD_H) { /* uglys*/
		return (0);
	} else if (get_board_index(create_vector(y, x)) >= BOARD_SIZE) {
		return (0);
	}
	tile_state = get_tile_board_val(ipc->ptr, create_vector(y, x));
	if (tile_state != player->team_id && tile_state != TILE_EMPTY) {
		player->target = create_vector(y, x);
		return (1);
	}	
	return (0);
}

/**
 * @brief Find enemy in range
 * @param ipc The ipc structure
 * @param player The player structure
 * @param range_max The range to scan
 * @return 1 and set player->targer if enemy found, otherwise return 0
*/
int8_t find_enemy_inXrange(t_ipc *ipc, t_player *player, int range_max)
{
	// uint32_t	tile_state = TILE_EMPTY;
	t_vec		pos = player->pos;

	for (int x_change = 1; x_change <= range_max; ++x_change) {
		for (int y_change = 1; y_change <= range_max; ++y_change) {
			uint32_t add_y = pos.y + y_change;
			uint32_t add_x = pos.x + x_change;
			uint32_t sub_y = pos.y - y_change;
			uint32_t sub_x = pos.x - x_change;
			// ft_printf_fd(2, CYAN"Test [%u] [%u]\n"RESET, add_y, add_x);
			if (is_enemy_tile(ipc, player, pos.x, add_y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, pos.x, sub_y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, add_x, pos.y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, sub_x, pos.y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, add_x, add_y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, sub_x, sub_y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, add_x, sub_y)) {
				return (1);
			} else if (is_enemy_tile(ipc, player, sub_x, add_y)) {
				return (1);
			}
		}
	}
	return (0);
}

// static int8_t is_suicide_tile()
// {

// }

void player_waiting(t_ipc *ipc, t_player *player)
{
	uint32_t		to_rush = extract_msg(ipc, player);
	
	t_vec rush_vec = get_board_pos(to_rush);
	// uint32_t test = 0; store heuristic from mssg here 
	t_heuristic	hp = {UINT32_MAX, create_vector(rush_vec.y, rush_vec.x)};
	if (to_rush != UINT32_MAX) {
		hp.cost = get_heuristic_cost(player->pos, rush_vec);
	}

	/* Bool enemy found */
	int8_t enemy_found = (vector_cmp(player->target, player->pos) == 0);
	/* This enemy heuristic */
	uint32_t enemy_heuristic = get_heuristic_cost(player->pos, player->target);
	if (!enemy_found) {
		enemy_heuristic = UINT32_MAX;
	}
	
	if (!enemy_found && to_rush == UINT32_MAX) { /* If no enemy found and no message receive */
		ft_printf_fd(2, CYAN"Player %u stay in WAITING state, go to random pos, don't send msg\n"RESET, player->team_id);
		player->target = get_random_point(ipc->ptr, player->pos);
		return ;
	} else if ((to_rush == UINT32_MAX && enemy_found) || (enemy_heuristic < hp.cost)) { /* If no message receive but enemy found || If heuristic of the nearest enemy is better */
		ft_printf_fd(2, PURPLE"Player in team [%u] enter in TRACKER state, go to nearest enemy, send msg\n"RESET, player->team_id);
		player->state = S_TRACKER;
		if (to_rush != UINT32_MAX) {
			send_msg(ipc, player, to_rush);
		}
		send_msg(ipc, player, get_board_index(player->target));
		return ;
	} else if (enemy_heuristic >= hp.cost) { /* If heuristic of the nearest enemy is worst thans msg receive */
		ft_printf_fd(2, GREEN"Player in team [%u] enter in FOLLOWER state, go to msg enemy, don't send msg\n"RESET, player->team_id);
		player->state = S_FOLLOWER;
		player->target = create_vector(rush_vec.y, rush_vec.x);
		return ;
	} else { /* else really ?*/
		ft_printf_fd(2, RED"Player %u stay in WAITING state, else case mandatory ?????? \n"RESET, player->team_id);
		player->target = create_vector(rush_vec.y, rush_vec.x);
		// send_msg(ipc, player, rush_vec);  send message here
		return ;
	}
}
// }


/*
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


// t_vec find_enemy_inrange(t_ipc *ipc, t_vec pos, uint32_t team_id)
// {
// 	uint32_t	tile_state = TILE_EMPTY;
// 	t_vec		arround[] = ARROUND_VEC3_ARRAY(pos);

// 	for (int i = 0; i < 48; i++) {
// 		if (arround[i].x >= (BOARD_W) || arround[i].y >= BOARD_H) { /* uglys*/
// 			// ft_printf_fd(2, RED"1  [%u] [%u]\n"RESET, arround[i].y, arround[i].x);
//             continue;
//         } else if (get_board_index(arround[i]) >= BOARD_SIZE) {
// 			// ft_printf_fd(2, RED"2  [%u] [%u]\n"RESET, arround[i].y, arround[i].x);
// 			continue;
// 		}
// 		ft_printf_fd(2, CYAN" [%u] [%u]\n"RESET, arround[i].y, arround[i].x);

// 		tile_state = get_tile_board_val(ipc->ptr, arround[i]);
// 		if (tile_state != team_id && tile_state != TILE_EMPTY) {
// 			return (arround[i]);
// 		}
// 	}
// 	return (pos);
// }

// int8_t scan_board_arround(t_ipc *ipc, t_player *player, uint32_t range_max)
// {
// 	uint32_t	tile_state = TILE_EMPTY;

// 	(void)tile_state;
// 	(void)range_max;
// 	// for (uint32_t range = 1; range <= range_max; range++) {
// 	player->target = find_enemy_inrange(ipc, player->pos, player->team_id);
// 	if (!vector_cmp(player->target, player->pos)) {
// 		return (1);
// 	}
// 	// }
// 	return (0);
// }
