# include "../include/lem_ipc.h"

/**
 * @brief set tile board value
 * @param array (board)
 * @param position vector
 * @param value to set
*/
void set_tile_board_val(u_int16_t *array, t_vec vec, u_int16_t value)
{
	int idx = -1;
	if (vec.x >= BOARD_W || vec.y >= BOARD_H) {
		ft_printf_fd(2, RED"Error: set_tile_board_val: invalid vector y: [%d] x: [%d]\n"RESET, vec.y, vec.x);
		return ;
	}

	idx = (vec.y * BOARD_W) +  vec.x;
	// ft_printf_fd(1, "idx: %d\n", idx);
	array[idx] = value;
}

/**
 * @brief display uint16_t array
*/
void display_uint16_array(u_int16_t *array)
{
	int i = 0, j = 0;

	for (i = 0; i < BOARD_H; i++) {
		for (j = 0; j < BOARD_W; j++) {
			int idx = i * BOARD_W + j;
			ft_printf_fd(1, YELLOW"[%d] "RESET, array[idx]);
		}
		ft_printf_fd(1, "\n");
	}
}

int main(int argc, char **argv) 
{
	t_ipc	ipc = {};
	int		ret = 0;

	if (argc != 2) {
		ft_printf_fd(2, "Usage: %s <TEAM_ID>\n", argv[0]);
		return (1);
	}
	ft_printf_fd(1, "Ipc value: shmid: %d, %d, %d \n", ipc.shmid, ipc.key, ipc.ptr);
	ipc.shmid = init_shared_memory(&ipc, argv[0]);
	if (ipc.shmid == -1) {
		return (1);
	}
	ret = attach_shared_memory(&ipc);
	if (ret == -1) {
		return (1);
	}
	ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
	

	set_tile_board_val(ipc.ptr, create_vector(0, 0), 2);
	set_tile_board_val(ipc.ptr, create_vector(0, 1), 1);
	set_tile_board_val(ipc.ptr, create_vector(0, 2), 3);
	set_tile_board_val(ipc.ptr, create_vector(1, 0), 9);
	set_tile_board_val(ipc.ptr, create_vector(9, 9), 5);
	set_tile_board_val(ipc.ptr, create_vector(10, 10), 5);
	display_uint16_array(ipc.ptr);

	ret = detach_shared_memory(&ipc);
	
	ft_printf_fd(1, "Test main ok, id %d\n", ipc.shmid);
	ret = destroy_shared_memory(ipc.shmid);
	return (ret);
}