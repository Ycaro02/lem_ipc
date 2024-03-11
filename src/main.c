# include "../include/lem_ipc.h"

void display_uint16_array(u_int16_t *array)
{
	int i = 0, j = 0;

	for (i = 0; i < BOARD_H; i++) {
		for (j = 0; j < BOARD_W; j++) {
			ft_printf_fd(1, YELLOW"[%d] "RESET, array[i + j]);
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
	
	ipc.ptr[0] = 4;
	display_uint16_array(ipc.ptr);

	ret = detach_shared_memory(&ipc);
	
	ft_printf_fd(1, "Test main ok, id %d\n", ipc.shmid);
	ret = destroy_shared_memory(ipc.shmid);
	return (ret);
}