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
// # define BOARD_H 30U
# define BOARD_H 20U

/* Map width */
// # define BOARD_W 65U
# define BOARD_W 40U

/* Board size */
# define BOARD_SIZE (BOARD_H * BOARD_W)

/* Out of board index */
# define OUT_OF_BOARD (BOARD_SIZE + 1)

# define TEAM_NB BOARD_SIZE /* offset team number */

/* Shared memory data size needed */
# define SHM_DATA_SIZE ((sizeof(uint32_t) * BOARD_SIZE) + sizeof(uint32_t)) /* (4 * (30 * 60)) + 4, last for team number */

/* Modulo data size % page size */
# define MOD_PAGESIZE (size_t) (SHM_DATA_SIZE % PAGE_SIZE)

/* Align data require on PAGE_SIZE (4096) */
# define ALIGN_SHARED_MEM (size_t) (MOD_PAGESIZE != 0 ? (SHM_DATA_SIZE + PAGE_SIZE - MOD_PAGESIZE) : SHM_DATA_SIZE)

/* Tile empty value */
# define TILE_EMPTY 0

/* Boolean value for init_game call */
# define DISPLAY_HANDLER 0
# define PLAYER 1


/* Define to handle increment decrement team number value */
# define ADD_TEAM	1
# define RM_TEAM	0

/* File used in ftok call */
# define IPC_NAME		"/tmp/lemipc_key"

/* Number used in ftok call */
# define IPC_PROJ_ID	42

/* Brut vector arround array declaration */
# define ARROUND_VEC_ARRAY(point) { \
	{point.x, point.y - 1}, \
	{point.x, point.y + 1}, \
	{point.x - 1, point.y}, \
	{point.x + 1, point.y}, \
	{point.x - 1, point.y - 1}, \
	{point.x - 1, point.y + 1}, \
	{point.x + 1, point.y - 1}, \
	{point.x + 1, point.y + 1} \
}

/* Int global for sigint handle game status */

extern int g_game_run;


/* Heuristic structure */
typedef struct s_heuristic {
	uint32_t	cost;
	t_vec		pos;
} t_heuristic;

/* Message buffer structure */
typedef struct s_msgbuf {
	long mtype;       	/* type of received/sent message (team id, each tean receive is own team message) */
	char mtext[4];    	/* msg content (uint32 val, board pos to rush )*/
} t_msgbuf ;

/* IPCs structure */
typedef struct s_ipc {
	uint32_t	*ptr;		/* Pointer to the shared memory, value is 0 for tile_empty or otherwise for player team id */
	key_t		key;		/* Key result ftok */
	int			shmid;		/* Shared memory id */
	int			semid;		/* Semaphore id */
	int			msgid;		/* Message queue id */
} t_ipc;

/* Player structure */
typedef struct s_player {
	t_vec		pos;		/* Player position */
	t_vec		next_pos; 	/* Next position */
	t_vec		target;		/* Target position */
	t_vec		ally_pos;	/* Closest Ally position */
	uint32_t	team_id;	/* Team id */
	int8_t		state;		/* Player state */
} t_player;

/* Player state */
enum e_player_state {
	S_WAITING,				/* Waiting state*/
	S_TRACKER,				/* Tracker state */
	S_FOLLOWER,				/* Follower state */
};

/* Direction enum */
enum e_direction {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	NORTH_EAST,
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
	DIR_MAX
};


# define ALLY_FLAG 0
# define ENEMY_FLAG 1

int8_t check_death(uint32_t *board, t_vec point, uint32_t team_id);

void player_tracker_follower(t_ipc *ipc, t_player *player);

void player_waiting(t_ipc *ipc, t_player *player);

uint32_t get_heuristic_cost(t_vec start, t_vec end);
t_vec find_smarter_possible_move(t_ipc *ipc, t_vec current, t_vec end, uint32_t team_id);


int8_t find_player_in_range(t_ipc *ipc, t_player *player, int range_max, int8_t flag);



void		team_handling(uint32_t *array, uint32_t team_id, int8_t add);
/* msg */
int8_t		remove_msg_queue(t_ipc *ipc);
uint32_t 	extract_msg(t_ipc *ipc, t_player *player);
int8_t		send_msg(t_ipc *ipc, t_player *player, uint32_t data);
int8_t clear_msg_queue(t_ipc *ipc, long team_id);
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
t_vec get_board_pos(uint32_t idx);
uint32_t	get_tile_board_val(uint32_t *array, t_vec vec);
uint32_t	get_board_index(t_vec vec);
void		display_uint16_array(uint32_t *array);
void 		set_tile_board_val(uint32_t *array, t_vec vec, uint32_t value);

/* player */
int			init_player(t_player *player, int argc, char **argv);
void 		player_routine(t_ipc *ipc, t_player *player);
/* random position */
t_vec get_random_point(uint32_t *array, t_vec player_pos);

# endif /* LEM_IPC_HEADER */ 