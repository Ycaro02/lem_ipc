# ifndef LEM_IPC_HEADER
# define LEM_IPC_HEADER 1

# include <sys/types.h>			/* Basic type */
# include <sys/ipc.h>			/* System V IPC */
# include <sys/shm.h>			/* Shared memory */

# include <errno.h>				/* Error handling */
# include <stdio.h>				/* Standard input/output */

# include <unistd.h>			/* Unix standard */
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

/* Shared memory data size */
# define SHM_DATA_SIZE ((sizeof(u_int16_t) * BOARD_SIZE))

/* Modulo data size % page size */
# define MOD_PAGESIZE (size_t) (SHM_DATA_SIZE % PAGE_SIZE)

/* Align data require on PAGE_SIZE (4096) */
# define ALIGN_SHARED_MEM (size_t) (MOD_PAGESIZE != 0 ? (SHM_DATA_SIZE + PAGE_SIZE - MOD_PAGESIZE) : SHM_DATA_SIZE)

/* Tile empty value */
# define TILE_EMPTY 0


typedef struct s_ipc {
	key_t		key;		/* Key result ftok */
	int			shmid;		/* Shared memory id */
	u_int16_t	*ptr;		/* Pointer to the shared memory, value is 0 for tile_empty or otherwise for player team id */
} t_ipc;

/* start ipc */
int		init_shared_memory(t_ipc *ipc, char *path);
int		destroy_shared_memory(int shmid);
int		attach_shared_memory(t_ipc *ipc);
int		detach_shared_memory(t_ipc *ipc);

/* handle board */
u_int16_t	get_board_index(t_vec vec);
void		display_uint16_array(u_int16_t *array);
void 		set_tile_board_val(u_int16_t *array, t_vec vec, u_int16_t value);


# endif /* LEM_IPC_HEADER */ 