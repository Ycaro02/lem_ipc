# include "../include/lem_ipc.h"

/**
 * @brief get board vector position
 * @param index
 * @return vector
*/
t_vec get_board_vec(u32 idx) {
	return (create_vector(idx / BOARD_W, idx % BOARD_W));
}

/**
 * @brief get board index
 * @param vector
 * @return index
*/
u32 get_board_index (t_vec vec)
{
	u32 idx = (vec.y * BOARD_W) + vec.x;
	
	if (idx >= BOARD_SIZE) {
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
void set_tile_board_val(u32 *array, t_vec vec, u32 value)
{
	int	idx = get_board_index(vec);

	if (idx == OUT_OF_BOARD) {
		ft_printf_fd(2, RED"Get_board_index error: invalid index: [%d]\n"RESET, idx);
		return ;
	}
	array[idx] = value;
}

/**
 *	@brief get tile board value
 *	@param array (board)
 *	@param position vector
 *	@return value
*/
u32 get_tile_board_val(u32 *array, t_vec vec)
{
	int	idx = get_board_index(vec);

	if (idx == OUT_OF_BOARD) {
		return (OUT_OF_BOARD);
	}
	return (array[idx]);
}

u32 get_playing_state(u32 *array) {
	return (array[GAME_PLAYING_STATE_IDX]);
}

void set_playing_state(u32 *array, u32 state) {
	array[GAME_PLAYING_STATE_IDX] = state;
}