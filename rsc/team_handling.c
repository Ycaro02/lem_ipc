# include "../include/lem_ipc.h"

void team_handling(uint32_t *array, uint32_t team_id, int8_t add)
{
	uint32_t i = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		if (array[i] == team_id) {
            return ;
		}
	}
    if (add) {
        array[TEAM_NB] += 1U;
    } else if (array[TEAM_NB] > 0U) {
        array[TEAM_NB] -= 1U;
    }
}

