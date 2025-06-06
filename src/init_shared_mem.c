# include "../include/lem_ipc.h"

/**
 *	@brief Initialize a shared memory segment
 *	@param path Path to the file to use ftok
 *	@return The shared memory id
*/
int get_shared_memory(key_t key, int flag)
{
	int shmid;

	errno = 0;
	/* IPC_EXCL: Fail if key exists protect against double creation */
	shmid = shmget(key, ALIGN_SHARED_MEM, (flag | 0666));
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	return (shmid);
}

/**
 * @brief get number of attached process
*/
int get_attached_processnb(IPC *ipc)
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
int attach_shared_memory(IPC *ipc)
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
int detach_shared_memory(IPC *ipc)
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
int clean_shared_rsc(IPC *ipc)
{
	int ret = 0;


	ret = detach_shared_memory(ipc);
	if (ret == -1) {
		return (ret);
	}
	ret = destroy_shared_memory(ipc->shmid);
	remove_msg_queue(ipc);
	sem_unlock(ipc->semid);
	destroy_semaphore_set(ipc->semid);
	return (ret);
}
