# include "../include/lem_ipc.h"

/**
 *	@brief Print an error message and the error string of a syscall
 *	@param syscall_name The name of the syscall
*/
void syscall_perror(char *syscall_name)
{
	ft_printf_fd(2, "%s failed\n", syscall_name);
	perror(syscall_name);
}

/**
 *	@brief Initialize a shared memory segment
 *	@param path Path to the file to use ftok
 *	@return The shared memory id
*/
int init_shared_memory(t_ipc *ipc)
{
	int shmid;

	errno = 0;
	/* IPC_EXCL: Fail if key exists protect against double creation */
	shmid = shmget(ipc->key, ALIGN_SHARED_MEM, (IPC_CREAT | IPC_EXCL | 0666));
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	// ft_printf_fd(1, GREEN"Mem size required: %d allocated: %d\n"RESET, SHM_DATA_SIZE, ALIGN_SHARED_MEM);
	return (shmid);
}


/**
 * @brief get number of attached process
*/
int get_attached_processnb(t_ipc *ipc)
{
	struct shmid_ds buf;

	errno = 0;
	if (shmctl(ipc->shmid, IPC_STAT, &buf) == -1) {
		syscall_perror("shmctl");
		return (-1);
	}
	return (buf.shm_nattch);
}

/**
 *	@brief Attach a shared memory segment
 *	@param ipc The ipc structure
 *	@return 0 on success, -1 on error
*/
int attach_shared_memory(t_ipc *ipc)
{
	errno = 0;
	ipc->ptr = shmat(ipc->shmid, NULL, 0);
	if (ipc->ptr == (void *)-1) {
		syscall_perror("shmat");
		return (-1);
	}
	return (0);
}

/**
 *	@brief Detach a shared memory segment
 *	@param ipc The ipc structure
 *	@return 0 on success, -1 on error
*/
int detach_shared_memory(t_ipc *ipc)
{
	errno = 0;
	if (shmdt(ipc->ptr) == -1) {
		syscall_perror("shmdt");
		return (-1);
	}
	return (0);
}

/**
 *	@brief Destroy a shared memory segment
 *	@param shmid The shared memory id
 *	@return 0 on success, -1 on error
*/
static int destroy_shared_memory(int shmid)
{
	errno = 0;
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	return (0);
}

/**
 *	@brief Clean the shared memory
 *	@param ipc The ipc structure
 *	@return 0 on success, -1 on error
*/
int clean_shared_memory(t_ipc *ipc)
{
	int ret = 0;

	ret = detach_shared_memory(ipc);
	if (ret == -1) {
		return (ret);
	}
	ret = destroy_shared_memory(ipc->shmid);

	destroy_semaphore_set(ipc->semid);

	return (ret);
}
