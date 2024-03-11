# include "../include/lem_ipc.h"

# include <stdint.h>


int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	int			ret = 0;

	if (argc != 2) {
		ft_printf_fd(2, "Usage: %s <TEAM_ID>\n", argv[0]);
		return (1);
	}

	uint64_t id_check = array_to_uint32(argv[1]);
	if (id_check == OUT_OF_UINT32) {
		ft_printf_fd(2, "Invalid team id\n");
		return (1);
	}
	ft_printf_fd(1, "Team id: %u\n", (uint32_t)id_check);
	player.team_id = (uint32_t)id_check;
	(void)player;


	ft_printf_fd(1, "Ipc value: shmid: %u, %u, %p \n", ipc.shmid, ipc.key, ipc.ptr);
	ipc.shmid = init_shared_memory(&ipc, argv[0]);
	if (ipc.shmid == -1) {
		return (1);
	}
	ret = attach_shared_memory(&ipc);
	if (ret == -1) {
		return (1);
	}
	ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
	

	set_tile_board_val(ipc.ptr, create_vector(0, 0), player.team_id);
	set_tile_board_val(ipc.ptr, create_vector(0, 1), player.team_id);
	set_tile_board_val(ipc.ptr, create_vector(0, 2), 3);
	set_tile_board_val(ipc.ptr, create_vector(1, 0), 9);
	set_tile_board_val(ipc.ptr, create_vector(9, 9), 5);
	display_uint16_array(ipc.ptr);

	ret = detach_shared_memory(&ipc);
	
	ft_printf_fd(1, "Test main ok, id %d\n", ipc.shmid);
	ret = destroy_shared_memory(ipc.shmid);
	return (ret);
}