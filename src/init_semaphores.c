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


int get_shared_mem(t_ipc *ipc)
{
	/* if shared memory even created just atach segment */
	if (attach_shared_memory(ipc) == -1) {
			return (-1);
	}
	return (0);
}

int init_semaphores_set(t_ipc *ipc, char *path, int8_t allow)
{
	/* Same here active protection when debug start is finish */
	key_t key = file_to_key(path);
	
	if (key == -1) {
		return (-1);
	}
	ipc->key = key;
	ipc->semid = semget(ipc->key, 1, (IPC_CREAT | IPC_EXCL | 0666));
	if (allow == 0 && ipc->semid == -1) {
		syscall_perror("semget");
		return (-1);
	} else if ( ipc->semid == -1 ) {
		get_shared_mem(ipc);
		return (0);
	} 

	ipc->shmid = init_shared_memory(ipc);
	if (ipc->shmid == -1 || get_shared_mem(ipc) == -1) {
		return (-1);
	}
	// semop(ipc->semid, &(struct sembuf){0, SETVAL, 0}, 5);
	if (semctl(ipc->semid, 0, SETVAL, 0)) {
		syscall_perror("semctl");
		return (-1);
	}
	ft_printf_fd(1, YELLOW"Semaphore value: "RESET""CYAN"%d\n"RESET, semctl(ipc->semid, 0, GETVAL));
	ft_printf_fd(1, YELLOW"Ptr value: "RESET""CYAN"%p\n"RESET, ipc->ptr);
	
	return (0);
}