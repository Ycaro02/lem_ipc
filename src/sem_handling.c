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
 * @brief Lock a semaphore
 * @param semid The semaphore id
 * @return 0 on success, -1 on error
*/
int sem_lock(int semid) {
	struct sembuf sops = {0, -1, 0};
	return (semop(semid, &sops, 1));
}


/**
 * @brief Unlock a semaphore
 * @param semid The semaphore id
 * @return 0 on success, -1 on error
*/
int sem_unlock(int semid) {
	struct sembuf sops = {0, 1, 0};
	return (semop(semid, &sops, 1));
}