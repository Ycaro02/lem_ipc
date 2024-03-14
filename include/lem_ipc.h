# ifndef LEM_IPC_HEADER
# define LEM_IPC_HEADER 1

# include <sys/types.h>			/* Basic type */
# include <stdint.h>			/* Standard integer */
# include <sys/ipc.h>			/* System V IPC */
# include <sys/shm.h>			/* Shared memory */
# include <signal.h>			/* Signal */

# include <time.h>				/* Time for random gen */

# include <sys/sem.h>			/* Semaphore */
# include <sys/msg.h>			/* Message queue */

# include <errno.h>				/* Error handling */
# include <stdio.h>				/* Standard input/output used for perror */

# include "../libft/libft.h"	/* Libft */

/* MLX */
# include <math.h>
# include <fcntl.h>
# include "../minilibx-linux/mlx.h"
# include "../minilibx-linux/mlx_int.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                                                            //
//                                BOARD_SIZE                                  //
//                                                                            //
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* Explicitit define value for shared rsc handler return */
# define ERROR_CASE -1
# define CLIENT_CASE 1
# define SERVER_CASE 0

/* Return of getpagesize function casted in size_t 4096 */
# define PAGE_SIZE              (size_t)getpagesize()

/* Map height */
# define BOARD_H 20U

/* Map width */
# define BOARD_W 20U

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

/* Boolean value for init_game call */
# define DISPLAY_HANDLER 0
# define PLAYER 1

/* File used in ftok call */
# define IPC_NAME		"/tmp/lemipc_key"

/* Number used in ftok call */
# define IPC_PROJ_ID	42

/* game status */
extern int g_game_run;


typedef struct s_msgbuf {
	long mtype;       	/* type of received/sent message */
	char mtext[4];    	/* msg content */
} t_msgbuf ;

typedef struct s_ipc {
	key_t		key;		/* Key result ftok */
	int			shmid;		/* Shared memory id */
	uint32_t	*ptr;		/* Pointer to the shared memory, value is 0 for tile_empty or otherwise for player team id */
	int			semid;		/* Semaphore id */
	int			msgid;
} t_ipc;

typedef struct s_player {
	t_vec		pos;		/* Player position */
	t_vec		target;		/* Target position */
	uint32_t	team_id;	/* Team id */
} t_player;


/* msg */
int8_t		remove_msg_queue(t_ipc *ipc);

/* init semaphore */
int			init_game(t_ipc *ipc, char *path, int8_t allow);

/* sem handling */
int 		destroy_semaphore_set(int semid);
int 		sem_lock(int semid);
int 		sem_unlock(int semid);

/* init shared mem */
int			get_shared_memory(key_t key, int flag);
int			attach_shared_memory(t_ipc *ipc);
int			detach_shared_memory(t_ipc *ipc);
int 		clean_shared_rsc(t_ipc *ipc);
int			get_attached_processnb(t_ipc *ipc);

/* ipcs utils*/
void 		syscall_perror(char *syscall_name);

/* handle board */
uint32_t	get_tile_board_val(uint32_t *array, t_vec vec);
uint32_t	get_board_index(t_vec vec);
void		display_uint16_array(uint32_t *array);
void 		set_tile_board_val(uint32_t *array, t_vec vec, uint32_t value);

/* random position */
t_vec 		get_reachable_point(uint32_t *array);

# endif /* LEM_IPC_HEADER */ 