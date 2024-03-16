#include "../include/lem_ipc.h"

uint32_t get_heuristic_cost(t_vec start, t_vec end)
{
	uint32_t cost = 0;

	cost = abs_diff(start.x, end.x) + abs_diff(start.y, end.y);
	// ft_printf_fd(2, YELLOW"cost for %u %u to %u %u = %u\n"RESET, start.x, start.y, end.x, end.y, cost);
	return (cost);
}


t_heuristic find_smarter_possible_move(t_ipc *ipc, t_vec current, t_vec end)
{
	t_vec possible_move[DIR_MAX] = ARROUND_VEC_ARRAY(current);
	t_vec enemy[DIR_MAX] = ARROUND_VEC_ARRAY(end);

    t_heuristic best_heuristic = {UINT32_MAX, create_vector(current.y, current.x)};

    // t_vec best_end = end;

    uint32_t test = UINT32_MAX;

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
                    // best_end = enemy[j];
                }
			}
		}
	}
	return (best_heuristic);
}

t_vec find_enemy_inrange(t_ipc *ipc, t_vec pos, uint32_t team_id, uint32_t range)
{
	uint32_t	tile_state = TILE_EMPTY;
	t_vec		arround[DIR_MAX] = ARROUND_VECX_ARRAY(pos, range);


	for (int i = 0; i < DIR_MAX; i++) {
		if (get_board_index(arround[i]) >= BOARD_SIZE) {
			continue;
		}
		tile_state = get_tile_board_val(ipc->ptr, arround[i]);
		if (tile_state != team_id && tile_state != TILE_EMPTY) {
			return (arround[i]);
		}
	}
	return (pos);
}

int8_t scan_board_arround(t_ipc *ipc, t_player *player, uint32_t range_max)
{
	uint32_t	tile_state = TILE_EMPTY;

	(void)tile_state;
	for (uint32_t range = 1; range <= range_max; range++) {
		player->target = find_enemy_inrange(ipc, player->pos, player->team_id, range);
		if (!vector_cmp(player->target, player->pos)) {
			return (1);
		}
	}
	return (0);
}

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