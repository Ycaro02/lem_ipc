# include "../include/lem_ipc.h"


/* @brief Initialize player */

int init_player(t_player *player, int argc, char **argv)
{
	uint64_t id_check;

	if (argc != 2) {
		ft_printf_fd(2, "Usage: %s <TEAM_ID>\n", argv[0]);
		return (-1);
	}
	// player->pos = create_vector(0, 0);
	// player->target = create_vector(0, 0);
	id_check = array_to_uint32(argv[1]);
	if (id_check == OUT_OF_UINT32) {
		ft_printf_fd(2, "Invalid team id\n");
		return (-1);
	}
	ft_printf_fd(1, YELLOW"New player Join team number:%s %s[%u]%s\n", RESET, RED, (uint32_t)id_check, RESET);
	player->team_id = (uint32_t)id_check;
	return (0);
}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	int			ret = 0;


	if (init_player(&player, argc, argv) != 0) {
		return (1);
	}
	
	init_semaphores_set(&ipc, argv[0]);
	
	if (semget(ipc.key, 0, (IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		ft_printf_fd(1, "Semget -1 semaphore already created for this key ???\n");
		perror("semget");
	}

	ipc.shmid = init_shared_memory(&ipc);
	if (ipc.shmid == -1) {
		return (1);
	}
	ret = attach_shared_memory(&ipc);
	if (ret == -1) {
		return (1);
	}
	// ft_printf_fd(1, "ptr before %p\n", ipc.ptr);
	

	set_tile_board_val(ipc.ptr, create_vector(0, 0), player.team_id);
	set_tile_board_val(ipc.ptr, create_vector(0, 1), player.team_id);
	display_uint16_array(ipc.ptr);

	clean_shared_memory(&ipc);
	return (ret);
}