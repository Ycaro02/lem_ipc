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
// # define TILE_SIZE 28U
# define TILE_SIZE 20U

/* Right band tile number */
# define RIGHTBAND_TILE_NB 5U

/* Right band width in pixel */
# define RIGHTBAND_WIDTH (TILE_SIZE * RIGHTBAND_TILE_NB)

/* Screen width */
# define SCREEN_WIDTH (uint32_t)(TILE_SIZE * BOARD_W)

/* Screen height */
# define SCREEN_HEIGHT (uint32_t)(TILE_SIZE * BOARD_H)

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
	t_xvar		*mlx;		/* mlx handler */
	void		*win;		/* mlx window */
	t_img		img;		/* mlx image */
	t_ipc		*ipc;		/* ipc handler */
	t_list		*team;		/* team list */
	uint32_t	player_nb;	/* last team number store */
}	t_game;

/* global var to handle display with mlx*/
extern t_game *g_game;

typedef enum e_keyboard_key
{
	ESC = 65307,
	UP = 119,
	LEFT = 97,
	DOWN = 115,
	RIGHT = 100,
	ARROW_UP = 65362,
	ARROW_LEFT = 65361,
	ARROW_RIGHT = 65363,
	ARROW_DOWN = 65364,
}	t_keyboard_key;

void		free_team(void *team);
t_teamcolor	get_new_color(uint32_t current_team_nb, uint32_t team_id);
int8_t 		build_list_number_team(t_list **lst, uint32_t *array);

#endif /* DISPLAY_LEMIPC_H */ 