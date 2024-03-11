# ifndef LEM_IPC_HEADER
# define LEM_IPC_HEADER 1

# include <sys/types.h>			/* Basic type */
# include <stdint.h>			/* Standard integer */
# include <sys/ipc.h>			/* System V IPC */
# include <sys/shm.h>			/* Shared memory */

# include <sys/sem.h>			/* Semaphore */

# include <errno.h>				/* Error handling */
# include <stdio.h>				/* Standard input/output used for perror */

# include "../libft/libft.h"	/* Libft */

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                                                            //
//                                BOARD_SIZE                                  //
//                                                                            //
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* Return of getpagesize function casted in size_t 4096 */
# define PAGE_SIZE              (size_t)getpagesize()

/* Map height for NULL ptr */
# define BOARD_H 10

/* Map width for \0 bytes */
# define BOARD_W 10

/* Board size */
# define BOARD_SIZE (BOARD_H * BOARD_W)

/* Out of board index */
# define OUT_OF_BOARD (BOARD_SIZE + 1)

/* Shared memory data size needed */
# define SHM_DATA_SIZE ((sizeof(uint32_t) * BOARD_SIZE))

/* Modulo data size % page size */
# define MOD_PAGESIZE (size_t) (SHM_DATA_SIZE % PAGE_SIZE)

/* Align data require on PAGE_SIZE (4096) */
# define ALIGN_SHARED_MEM (size_t) (MOD_PAGESIZE != 0 ? (SHM_DATA_SIZE + PAGE_SIZE - MOD_PAGESIZE) : SHM_DATA_SIZE)

/* Tile empty value */
# define TILE_EMPTY 0

typedef struct s_ipc {
	key_t		key;		/* Key result ftok */
	int			shmid;		/* Shared memory id */
	uint32_t	*ptr;		/* Pointer to the shared memory, value is 0 for tile_empty or otherwise for player team id */
	int			semid;		/* Semaphore id */
} t_ipc;

typedef struct s_player {
	t_vec		pos;		/* Player position */
	t_vec		target;		/* Target position */
	uint32_t	team_id;	/* Team id */
} t_player;

/* init semaphore */
int			init_semaphores_set(t_ipc *ipc, char *path, int8_t allow);
int 		destroy_semaphore_set(int semid);

/* start ipc */
int			init_shared_memory(t_ipc *ipc);
int			attach_shared_memory(t_ipc *ipc);
int			detach_shared_memory(t_ipc *ipc);
int 		clean_shared_memory(t_ipc *ipc);
/* utils*/
void syscall_perror(char *syscall_name);
/* utils*/
/* handle board */
uint32_t	get_board_index(t_vec vec);
void		display_uint16_array(uint32_t *array);
void 		set_tile_board_val(uint32_t *array, t_vec vec, uint32_t value);



# endif /* LEM_IPC_HEADER */ 