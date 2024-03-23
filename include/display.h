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



// # define TOPBAND_HEIGHT (TILE_SIZE * 2)

// # define BOARD_PWIDTH (TILE_SIZE * BOARD_W)
# define COLOR_MAX 26U

/* Tile size in pixel */
# define TILE_SIZE 28U
// # define TILE_SIZE 20U


/* Right band tile number */
# define RIGHTBAND_TILE_NB 5U


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

/* Team color struct */
typedef struct s_teamcolor {
	char		*strcolor;	/* Team color name */
	int			color;		/* Team color value */
} t_teamcolor;

typedef struct s_team {
	uint32_t	tid;		/* Team Id */
	char		*strid;		/* Team Id String */
	uint32_t	tsize;		/* Team Size */
	char		*strsize;	/* Team Size String */
	uint32_t	kill;		/* Team Kill */
	t_teamcolor	data;		/* Team color ? */
} t_team;

typedef struct s_game
{
	t_xvar		*mlx;			/* mlx handler */
	void		*win;			/* mlx window */
	t_img		img;			/* mlx image */
	t_ipc		*ipc;			/* ipc handler */
	t_list		*player_data;	/* player data list */
	t_pdata		*selected;		/* player data */
	uint32_t	player_nb;		/* last team number store */
	t_vec		mouse_pos;		/* mouse position, TODO can remove*/
	int8_t		pause;			/* game pause bool */
	int8_t sem_lock;			/* sem is locked by display handler*/
}	t_game;

/* Key */
typedef enum e_keyboard_key
{
	LEFT_CLICK = 1,			/* Left click value */
	ESC = 65307,			/* Escape key value */
}	t_keyboard_key;

/*lst utils in parsepdata*/
int is_same_node(void *node, void *target);

/* click_event */
t_vec	get_click_tile(t_vec mouse);
int check_mouse(int keycode, int x, int y, t_game *game);

/* parse pdata */
void	receive_player_data(t_game *game);
void	*get_player_node(t_list *lst, t_vec target);
int8_t	extract_controle_packet(t_game *game);

/* display pdata */
void	display_pdata_lst(t_list *player_lst);
void	display_pdata_node(t_game *game, t_pdata *pdata, uint32_t y);

/* display team */
// void		free_team(void *team);
// int8_t 		build_list_number_team(t_list **lst, uint32_t *array);


#endif /* DISPLAY_LEMIPC_H */ 