# include "../include/lem_ipc.h"

int init_shared_memory(char *path)
{
	key_t key;
	int shmid;

	errno = 0;
	key = ftok(path, 42);
	if (key == -1) {
		ft_printf_fd(2, "ftok failed\n");
		perror("ftok");
		return (-1);
	}
	shmid = shmget(key, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
	if (shmid == -1) {
		ft_printf_fd(2, "shmget failed\n");
		return (-1);
	}
	return (shmid);
}

int destroy_shared_memory(int shmid)
{
	errno = 0;
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		ft_printf_fd(2, "shmctl failed\n");
		perror("shmctl");
		return (-1);
	}
	return (0);
}
