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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                                                            //
//                                BOARD_SIZE                                  //
//                                                                            //
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* Explicitit define value for shared rsc handler return */
enum e_rsc_handler_ret {
	ERROR_CASE=-1,
	CLIENT_CASE=1,
	SERVER_CASE=0,
};

/* Display handler state */
enum e_display_handler_state {
	DH_DISCONNECTED=0,
	DH_CONNECTED,
	DH_PRIORITY,
};

/* Control packet value */
enum e_ctrl_packet_val {
	CTRL_DH_WAITING_TO_CONNECT=0,	/* Player waiting for display handler connection */
	CTRL_DH_PRIORITY,
};

/* Return of getpagesize function casted in size_t 4096 */
#define PAGE_SIZE              (size_t)getpagesize()

/** 
	* Following data are used to define the board size and the tile size
	* Map height (nb_tile)
	* Tile size in pixel
	* Right band (nb_tile)
	* Map width (nb_tile)
	* Can be handle by the following define:
	* - LEMIPC_LITLE
	* - LEMIPC_VERY_LITTLE
*/

#if defined(LEMIPC_LITLE)
	/* Little size: 10,000 tile */
	#define BOARD_H 100U
	#define TILE_SIZE 8U
	#define RIGHTBAND_TILE_NB (25U)
#elif defined(LEMIPC_VERY_LITTLE)
	/* Verry little size: 40,000 tile */
	#define BOARD_H 200U
	#define TILE_SIZE 4U
	#define RIGHTBAND_TILE_NB 60U
#else
	/* Big size: 3,600 tile */
	#define BOARD_H 60U
	#define TILE_SIZE 14U
	#define RIGHTBAND_TILE_NB 15U
	// #define BOARD_H 10U
	// #define TILE_SIZE 35U
	// #define RIGHTBAND_TILE_NB 5U
#endif

#define BOARD_W BOARD_H

#define PLAYER_WAIT_TIME 100000

/* Size max of the message queue in bytes */
#define MSG_QUEUE_SIZE 16384

/* Limit size of the message queue before set display handler priority and wait for him */
#define MSG_QUEUE_LIMIT_SIZE 10000

/* Board size */
#define BOARD_SIZE (BOARD_H * BOARD_W)

/* Out of board index */
#define OUT_OF_BOARD (BOARD_SIZE + 1)
#define GAME_PLAYING_STATE_IDX OUT_OF_BOARD

/* Display handler id */
#define DISPLAY_HANDLER_CHAN	UINT32_MAX
#define DISPLAY_HANDLER_ID		UINT32_MAX

/* Display controle chanel, ned to remove this value to accepted team val */
#define CONTROLE_DISPLAY_CHAN (u32)(DISPLAY_HANDLER_CHAN - 1U)

/* Team id max, UINT32_MAX used for error value/display_handler ID, and UINT32_MAX - 1 for controle display channel */
#define TEAM_ID_MAX (CONTROLE_DISPLAY_CHAN - 1U)

/* Shared memory data size needed */
#define SHM_DATA_SIZE (sizeof(u32) * BOARD_SIZE + 1) /* (4 * (BOARD_SIZE)) */

/* Modulo data size % page size */
#define MOD_PAGESIZE (size_t) (SHM_DATA_SIZE % PAGE_SIZE)

/* Align data require on PAGE_SIZE (4096) */
#define ALIGN_SHARED_MEM (size_t) (MOD_PAGESIZE != 0 ? (SHM_DATA_SIZE + PAGE_SIZE - MOD_PAGESIZE) : SHM_DATA_SIZE)

/* Tile empty value */
#define TILE_EMPTY 0

/* Boolean value for init_game call */
#define DISPLAY_HANDLER 0
#define PLAYER 1

/* DIR MAX*/
#define DIR_MAX 8

/* Ally flag for find_player_in_range */
#define ALLY_FLAG 0
/* Enemy flag for find_player_in_range */
#define ENEMY_FLAG 1

/* Alive value to avoid magic number */
#define ALIVE		0

/* File used in ftok call */
#define IPC_NAME		"/tmp/lemipc_key"

/* Number used in ftok call */
#define IPC_PROJ_ID	42

/* Brut vector arround array declaration */
#define ARROUND_VEC_ARRAY(point) { \
	{point.x, point.y - 1}, \
	{point.x, point.y + 1}, \
	{point.x - 1, point.y}, \
	{point.x + 1, point.y}, \
	{point.x - 1, point.y - 1}, \
	{point.x - 1, point.y + 1}, \
	{point.x + 1, point.y - 1}, \
	{point.x + 1, point.y + 1} \
}

/* Heuristic structure */
typedef struct s_heuristic {
	u32	cost;
	t_vec		pos;
} Heuristic;

/* Message buffer structure */
typedef struct s_msgbuf {
	long mtype;       	/* type of received/sent message (team id, each tean receive is own team message) */
	char mtext[4];    	/* msg content (uint32 val, board pos to rush )*/
} MsgBuf ;

/* IPCs structure */
typedef struct s_ipc {
	u32			*ptr;		/* Pointer to the shared memory, value is 0 for tile_empty or otherwise for player team id */
	key_t		key;		/* Key result ftok */
	int			shmid;		/* Shared memory id */
	int			semid;		/* Semaphore id */
	int			msgid;		/* Message queue id */
	s8			display;	/* Display handler conected */
} IPC;

/* Player structure */
typedef struct s_player {
	t_vec		pos;		/* Player position */
	t_vec		next_pos; 	/* Next position */
	t_vec		target;		/* Target position */
	t_vec		ally_pos;	/* Closest Ally position */
	u32			team_id;	/* Team id */
	u32			kill_by;	/* Kill by team id */
	s8			state;		/* Player state */
} Player;


/* Player data packet init */
#define INIT_PDATA_PACKET { \
	{"player data start", {UINT32_MAX}}, \
	{"player data state", {UINT32_MAX}}, \
	{"player data team id", {UINT32_MAX}}, \
	{"player data pos", {UINT32_MAX}}, \
	{"player data target", {UINT32_MAX}}, \
	{"player data closest ally", {UINT32_MAX}}, \
	{"player data supp", {UINT32_MAX}} \
}


/* Controle packet macro builder */
#define BUILD_CTRL_PACKET(x) {x, x, x, x, x, x, x}

/* Message type extract */
#define GET_MSG_TYPE(state) (state & 0b11111000)

/* Message player state extract */
#define GET_MSG_STATE(state) (state & 0b00000111)

/* 
	Message packets for display is build as follow:
		- send 0 for message start
		- send state : (e_msg_type | s_player_state), access with GET_MSG_TYPE/STATE
		- send player team id
		- send player position		(index uint32) Identification field for all type exept create
		- send player target pos	(index uint32)
		- send player ally pos		(index uint32)
		- send supp data only for update POS (new pos)
	When the packet is read by the display handler, it will store data as follow:
		- Read start packet dont't store it
		- Read state packet, extract type and state with macro to store it
		- Read team id, store it as sdata field
		- Read position, store it as vdata field
		- Read target, store it as vdata field
		- Read ally, store it as vdata field
		- Read supp data if needed, store it as vdata field
		- Display packet	
*/

/* Player data structure */
typedef struct s_player_data {
	char			*name;	/* Data name field */
	union {
		u32	sdata;	/* Data simple value */
		t_vec		vdata;	/* Data vector value */
	}; 
} PlayerData;

/* Player data index */
enum e_pdata_idx {
	PDATA_START=0U,			/* Start packet */
	PDATA_STATE,			/* Player state mixed with message type */
	PDATA_TID,				/* Player team id */
	PDATA_POS,				/* Player position */
	PDATA_TARGET,			/* Player target */
	PDATA_ALLY,				/* Player closest ally */
	PDATA_SUPP,				/* Supp data for update pos or kill counter */
	PDATA_LEN,				/* Packet len */
};

/* Player state */
enum e_player_state {
	S_WAITING=(1U << 0),	/* Waiting state*/
	S_TRACKER=(1U << 1),	/* Tracker state */
	S_FOLLOWER=(1U << 2),	/* Follower state */
};

/* Message type */
enum e_msg_type {
	P_CREATE=(1U << 3),		/* Create player */
	P_UPDATE_POS=(1U << 4), /* Update player position */
	P_DELETE=(1U << 5),		/* Delete player */
};


/* Int global for sigint handle game status */
extern int g_game_run;


/* main */
u32	check_death(u32 *board, t_vec point, u32 team_id);

/* player move */
void		player_tracker_follower(IPC *ipc, Player *player);
t_vec		find_smarter_possible_move(IPC *ipc, t_vec current, t_vec end);
void		player_waiting(IPC *ipc, Player *player);
u32			get_heuristic_cost(t_vec start, t_vec end);
s8			find_player_in_range(IPC *ipc, Player *player, u32 range_max, s8 flag);

/* send pdata */
void		send_display_controle_packet(IPC *ipc, u32 displayer_state, u32 from_id);
s8			display_handler_state(IPC *ipc);
void		send_pdata_display(IPC *ipc, Player *player, u8 msg_type);

// display handlker waiing
void		wait_for_display_handler_priority(IPC *ipc);
void		wait_for_display_handler_connect(IPC *ipc);

/* message queue */
int			get_msg_queue(key_t key, int flag);
s8			remove_msg_queue(IPC *ipc);
u32 		extract_msg(IPC *ipc, u32 msg_id);
s8			send_msg(IPC *ipc, u32 msg_id, u32 data, u32 from_id);
s8			clear_msg_queue(IPC *ipc, long chan_id);
u32			message_queue_size_get(int msgid);
/* init semaphore */
int			init_game(IPC *ipc, char *path, s8 allow);

/* sem handling */
int 		destroy_semaphore_set(int semid);
int 		sem_lock(int semid);
int 		sem_unlock(int semid);

/* init shared mem */
int			get_shared_memory(key_t key, int flag);
int			attach_shared_memory(IPC *ipc);
int			detach_shared_memory(IPC *ipc);
int 		clean_shared_rsc(IPC *ipc);
int			get_attached_processnb(IPC *ipc);

/* ipcs utils*/
void 		syscall_perror(char *syscall_name);

/* handle board */
t_vec		get_board_vec(u32 idx);
u32			get_tile_board_val(u32 *array, t_vec vec);
u32			get_board_index(t_vec vec);
void 		set_tile_board_val(u32 *array, t_vec vec, u32 value);

/* player */
int			init_player(Player *player, int argc, char **argv);
int 		player_routine(IPC *ipc, Player *player);
int			init_signal_handler(void);

/* random position */
t_vec		get_random_point(u32 *array);
u32 		get_playing_state(u32 *array);
void 		set_playing_state(u32 *array, u32 state);



# endif /* LEM_IPC_HEADER */ 