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
int init_shared_memory(char *path)
{
	key_t key;
	int shmid;

	errno = 0;
	key = ftok(path, 42);
	if (key == -1) {
		syscall_perror("ftok");
		return (-1);
	}
	errno = 0;
	/* IPC_EXCL: Fail if key exists protect against double creation ? */
	shmid = shmget(key, SHARED_MEMORY_SIZE, (IPC_CREAT | IPC_EXCL | 0666));
	if (shmid == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	return (shmid);
}

/**
 *	@brief Destroy a shared memory segment
 *	@param shmid The shared memory id
 *	@return 0 on success, -1 on error
*/
int destroy_shared_memory(int shmid)
{
	errno = 0;
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		syscall_perror("shmget");
		return (-1);
	}
	return (0);
}
