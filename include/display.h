# ifndef DISPLAY_LEMIPC_H
# define DISPLAY_LEMIPC_H

# include "lem_ipc.h"
# include "handle_sdl.h"

# define RED_INT 0xFF0000FF
# define GREEN_INT 0x00FF00FF
# define TURQUOISE_INT 0x03FC98FF
# define YELLOW_INT 0xFFFF00FF
# define BLUE_INT 0x0000FFFF
# define PURPLE_INT 0x800080FF
# define CYAN_INT 0x00FFFFFF
# define ORANGE_INT 0xFFA500FF
# define ORANGE2_INT 0xFCBA03FF 
# define PINK_INT 0xFFC0CBFF
# define BROWN_INT 0xA52A2AFF
# define BLACK_INT 0x000000FF


/* Define for team handling flag */
# define REMOVE_TEAM 0
# define JOIN_TEAM 1
# define UPDATE_KILL 2 

/* Max team display in rightband */
# define MAX_TEAM_DISPLAY 5

// # define BOARD_PWIDTH (TILE_SIZE * BOARD_W)
# define COLOR_MAX 26U

/* Right band width in pixel */
# define RIGHTBAND_WIDTH (TILE_SIZE * RIGHTBAND_TILE_NB)

# define TOTAL_SCREEN_WIDTH ((TILE_SIZE * BOARD_W) + RIGHTBAND_WIDTH)

/* Screen width */
# define SCREEN_WIDTH TOTAL_SCREEN_WIDTH

/* Screen height */
# define SCREEN_HEIGHT (TILE_SIZE * BOARD_H)

/* Right band start string position */
# define RB_START_STR_Y (15U)
# define RB_START_STR_X (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U)

/* Boolean flag for display board function */
#define EMPTY_BOARD TRUE
#define PLAYER_BOARD FALSE

/* Mouse click define value */
#define LEFT_CLICK 1

/* Team color struct */
typedef struct s_teamcolor {
	char		*strcolor;	/* Team color name */
	int			color;		/* Team color value */
} TeamColor;

typedef struct s_team {
	u32			tid;		/* Team Id */
	char		*strid;		/* Team Id String */
	u32			tsize;		/* Team Size */
	char		*strsize;	/* Team Size String */
	u32			kill;		/* Team Kill */
	char		*kill_str;	/* Team Kill String */
	TeamColor	color;		/* Team color  */
} Team;

typedef struct s_game {
	SDLHandle	*h;					/* SDL handler */
	IPC			*ipc;				/* ipc handler */
	t_list		*team_data;			/* team data list */
	t_list		*player_data;		/* player data list */
	PlayerData	*player_selected;	/* player data to display */
	t_vec		mouse_pos;			/* mouse position, can remove*/
	u32			player_nb;			/* last team number store */
	u32			kill_from_remove_team;	/* kill from remove team */ 
	int			ressource_state;		/* ressource */
	s8			pause;			/* game pause bool */
	s8			sem_locked;		/* sem state */
}	Game;

/* main */
TeamColor get_new_color(u32 team_id); 

/* team handling */
void team_handling(Game *game, u32 team_id, s8 cmd);
void free_team(void *team);

/*lst utils in parsepdata*/
int is_same_node(void *node, void *target);

/* click_event */
t_vec	get_click_tile(t_vec mouse);
int		check_mouse(int keycode, int x, int y, Game *game);

/* parse pdata */
void	receive_player_data(Game *game);
void	*get_player_node(t_list *lst, t_vec target);
s32		extract_controle_packet(Game *game);
void 	extract_priority_packet(Game *game);

/* display pdata */
void	display_pdata_node(Game *game, PlayerData *pdata, u32 y);

/* display_rightband */
void display_righband(Game *game, PlayerData *pdata);

/* display_routine */
int sdl_draw_loop(Game *game);

/* display_utils */
TeamColor get_newcolor(u32 team_id);
Game *getGame(void);
s8 is_left_click_down(SDL_Event event);
s8 is_key_pressed(SDL_Event event, s32 key);
void mouse_position_update(SDLHandle *h, int x, int y);
void sig_handler(int signum);
void handle_display_connect(Game *game);
void sdl_display_center_string(Game *game, SDLHandle *h, char *str, uint32_t color);

/* Main */
int destroy_windows(Game *game);
s32 event_handler(Game *game, SDLHandle *h);
void sdl_draw_board(Game *game, SDLHandle *h, s8 empty_draw);

#endif /* DISPLAY_LEMIPC_H */ 