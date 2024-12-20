# ifndef DISPLAY_LEMIPC_H
# define DISPLAY_LEMIPC_H

# include "lem_ipc.h"

# define RED_INT 0xFF0000
# define GREEN_INT 0x00FF00
# define YELLOW_INT 0xFFFF00
# define BLUE_INT 0x0000FF
# define PURPLE_INT 0x800080
# define CYAN_INT 0x00FFFF
# define ORANGE_INT 0xFFA500
# define PINK_INT 0xFFC0CB
# define BROWN_INT 0xA52A2A
# define BLACK_INT 0x000000


# define REMOVE_TEAM 0
# define JOIN_TEAM 1
# define UPDATE_KILL 2 

# define MAX_TEAM_DISPLAY 5

// # define TOPBAND_HEIGHT (TILE_SIZE * 2)

// # define BOARD_PWIDTH (TILE_SIZE * BOARD_W)
# define COLOR_MAX 26U

/* Right band width in pixel */
# define RIGHTBAND_WIDTH (TILE_SIZE * RIGHTBAND_TILE_NB)

# define TOTAL_SCREEN_WIDTH ((TILE_SIZE * BOARD_W) + RIGHTBAND_WIDTH)

/* Screen width */
# define SCREEN_WIDTH TOTAL_SCREEN_WIDTH

/* Screen height */
# define SCREEN_HEIGHT (TILE_SIZE * BOARD_H)


# define PLAYER_REMAIN "PLAYER REMAINING : "

/* Y padding between str in rightband*/
# define PAD_YTEAM 25U
/* Char len to pixel */
# define CHAR_TOPIXEL 6
/* Start x for write str in rightband */
# define START_STR_X (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U)


// # define PAUSE_BTN_ASSET "rsc/assets/pause_btn.xpm"


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

typedef struct s_game
{
	t_xvar		*mlx;			/* mlx handler */
	void		*win;			/* mlx window */
	t_img		img;				/* mlx image, represent all screen execpt right band */
	t_img		right_band;			/* right band image */
	IPC		*ipc;				/* ipc handler */
	t_list		*team_data;			/* team data list */
	t_list		*player_data;		/* player data list */
	PlayerData		*player_selected;	/* player data to display */
	t_vec		mouse_pos;			/* mouse position, can remove*/
	u32			player_nb;			/* last team number store */
	u32			kill_from_remove_team;	/* kill from remove team */ 
	int			ressource_state;		/* ressource */
	s8			pause;			/* game pause bool */
	s8			space_state;	/* team size */
}	Game;

/* Key */
typedef enum e_keyboard_key
{
	LEFT_CLICK = 1,			/* Left click value */
	SPACE = 32,				/* Space key value */
	ESC = 65307,			/* Escape key value */
}	E_KeyboardKey;

/* main */
TeamColor get_new_color(u32 team_id); 

/* team handling */
void team_handling(Game *game, u32 team_id, s8 cmd);
void free_team(void *team);

/*lst utils in parsepdata*/
int is_same_node(void *node, void *target);

/* click_event */
t_vec	get_click_tile(t_vec mouse);
int check_mouse(int keycode, int x, int y, Game *game);

/* parse pdata */
void	receive_player_data(Game *game);
void	*gePlayer_node(t_list *lst, t_vec target);
s8	extract_controle_packet(Game *game);
void 	extract_priority_packet(Game *game);
/* display pdata */
void	display_pdata_lst(t_list *player_lst);
void	display_pdata_node(Game *game, PlayerData *pdata, u32 y);

/* display team */
// void		free_team(void *team);
// s8 		build_list_number_team(t_list **lst, u32 *array);


#endif /* DISPLAY_LEMIPC_H */ 