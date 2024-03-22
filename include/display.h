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
	t_teamcolor	data;		/* Team color ? */
} t_team;

typedef struct s_game
{
	t_xvar		*mlx;			/* mlx handler */
	void		*win;			/* mlx window */
	t_img		img;			/* mlx image */
	t_ipc		*ipc;			/* ipc handler */
	t_list		*team;			/* team list TODO REMOVE adapt team display*/
	t_list		*player_data;	/* player data list */
	t_pdata		*selected;		/* player data */
	uint32_t	player_nb;		/* last team number store */
	t_vec		mouse_pos;		/* mouse position, TODO can remove*/
	int8_t		pause;			/* game pause bool */
}	t_game;

typedef enum e_keyboard_key
{
	LEFT_CLICK = 1,
	ESC = 65307,
}	t_keyboard_key;

void		free_team(void *team);
t_teamcolor	get_new_color(uint32_t current_team_nb, uint32_t team_id);
int8_t 		build_list_number_team(t_list **lst, uint32_t *array);


t_vec get_click_tile(t_vec mouse);

#endif /* DISPLAY_LEMIPC_H */ 