# include "../include/lem_ipc.h"


// static int set_sem_val(int semid, int val)
// {
// 	return (semctl(semid, 0, SETVAL, val));
// }

int destroy_semaphore_set(int semid)
{
	return (semctl(semid, 0, IPC_RMID));
}

// static int get_sem_val(int semid)
// {
// 	return (semctl(semid, 0, GETVAL));
// }

int file_to_key(char *path)
{
	key_t key;

	errno = 0;
	key = ftok(path, 42);
	if (key == -1) {
		syscall_perror("ftok");
		return (-1);
	}
	return (key);
}

int init_semaphores_set(t_ipc *ipc, char *path)
{
	/* Same here active protection when debug start is finish */
	// ipc->semid = semget(ipc->key, 1, (IPC_CREAT | IPC_EXCL | 0666));
	key_t key = file_to_key(path);
	if (key == -1) {
		return (-1);
	}
	ipc->key = key;
	ipc->semid = semget(ipc->key, 1, (IPC_CREAT | 0666));
	if (ipc->semid == -1) {
		syscall_perror("semget");
		return (-1);
	}
	// semop(ipc->semid, &(struct sembuf){0, SETVAL, 0}, 5);
	if (semctl(ipc->semid, 0, SETVAL, 0)) {
		syscall_perror("semctl");
		return (-1);
	}
	ft_printf_fd(1, YELLOW"Semaphore value: "RESET""CYAN"%d\n"RESET, semctl(ipc->semid, 0, GETVAL));
	
	return (0);
}