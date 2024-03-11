# ifndef LEM_IPC_HEADER
# define LEM_IPC_HEADER 1

# include <sys/types.h>			/* Basic type */
# include <sys/ipc.h>			/* System V IPC */
# include <sys/shm.h>			/* Shared memory */

# include <errno.h>				/* Error handling */
# include <stdio.h>				/* Standard input/output */

# include "../libft/libft.h"	/* Libft */


# define SHARED_MEMORY_SIZE 4096


int init_shared_memory(char *path);
int destroy_shared_memory(int shmid);



# endif /* LEM_IPC_HEADER */ 