# include "../include/lem_ipc.h"

/**
 * @brief get board index
 * @param vector
 * @return index
*/
uint32_t get_board_index (t_vec vec)
{
	int idx = (vec.y * BOARD_W) +  vec.x;
	if (idx < 0 || idx >= BOARD_SIZE) {
		ft_printf_fd(2, RED"Get_board_index error: invalid index: [%d]\n"RESET, idx);
		return (OUT_OF_BOARD);
	}
	return (idx);
}

/**
 * @brief set tile board value
 * @param array (board)
 * @param position vector
 * @param value to set
*/
void set_tile_board_val(uint32_t *array, t_vec vec, uint32_t value)
{
	int idx = get_board_index(vec);
	if (idx == OUT_OF_BOARD) {
		return ;
	}
	array[idx] = value;
}

/* @brief display board in stdout */
void display_uint16_array(uint32_t *array)
{
	int i = 0, j = 0;

	for (i = 0; i < BOARD_H; i++) {
		for (j = 0; j < BOARD_W; j++) {
			int idx = i * BOARD_W + j;
			ft_printf_fd(1, YELLOW"[%s%s%u%s%s] "RESET, RESET, CYAN, array[idx], RESET, YELLOW);
		}
		ft_printf_fd(1, "\n");
	}
}
