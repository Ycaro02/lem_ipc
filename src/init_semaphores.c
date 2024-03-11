# include "../include/lem_ipc.h"

/**
 * @brief Destroy a semaphore set
 * @param semid The semaphore id
 * @return 0 on success, -1 on error
*/
int destroy_semaphore_set(int semid) {
	return (semctl(semid, 0, IPC_RMID));
}


/**
 * @brief Get key from file path
 * @param path The file path
*/
static int file_to_key(char *path)
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


/**
 * @brief Get semaphore set id
 * @param key The key reference the semaphore
*/
static int get_sem_set_id(key_t key) 
{
	int shmid;

	errno = 0;
	shmid = semget(key, 1, 0666);
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	// ft_printf_fd(1, GREEN"Child get shared mem ID  %d\n"RESET, SHM_DATA_SIZE, ALIGN_SHARED_MEM);
	return (shmid);
}


/**
 *	@brief Get shared memory id
 *	@param key The key reference the shared memory
*/
static int get_shared_mem_id(key_t key) 
{
	int shmid;

	errno = 0;
	shmid = shmget(key, ALIGN_SHARED_MEM, 0666);
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	// ft_printf_fd(1, GREEN"Child get shared mem ID  %d\n"RESET, SHM_DATA_SIZE, ALIGN_SHARED_MEM);
	return (shmid);
}


int sem_detect_child(t_ipc *ipc, int8_t allow)
{
	ipc->semid = semget(ipc->key, 1, (IPC_CREAT | IPC_EXCL | 0666));
	if (allow == 0 && ipc->semid == -1) { /* if error and can't create sem (vizualizer case) */
		syscall_perror("semget");
		return (-1);
	} else if ( ipc->semid == -1 ) { /* if ressource already created */
		/* need to get semaphore first */
		ipc->shmid = get_shared_mem_id(ipc->key);
		ipc->semid = get_sem_set_id(ipc->key);
		if (ipc->shmid == -1 || ipc->semid == -1 || attach_shared_memory(ipc) == -1) {
			ft_printf_fd(2, RED"Error child can't get shared data"RESET);
		} 
		return (1); /* child case */
	} 
	return (0); /* parent case */
}

int init_semaphores_set(t_ipc *ipc, char *path, int8_t allow)
{
	/* Same here active protection when debug start is finish */
	ipc->key = file_to_key(path);
	if (ipc->key == -1) {
		ft_printf_fd(2, RED"Error can't get key for %s"RESET, path);
		return (-1);
	}
	if (sem_detect_child(ipc, allow) != 0) { /* if not parent */
		return (0);
	}
	/* Set semaphore value to 0 (lock it) */
	if (semctl(ipc->semid, 0, SETVAL, 0)) {
		syscall_perror("semctl");
		return (-1);
	}
	
	// ft_printf_fd(1, YELLOW"Semaphore value: "RESET""CYAN"%d\n"RESET, semctl(ipc->semid, 0, GETVAL));
	ipc->shmid = init_shared_memory(ipc);
	if (ipc->shmid == -1 || attach_shared_memory(ipc) == -1) {
		return (-1);
	}
	return (0);
}