# ifndef DISPLAY_LEMIPC_H
# define DISPLAY_LEMIPC_H

# include "lem_ipc.h"


// # define TOPBAND_HEIGHT (TILE_SIZE * 2)

// # define BOARD_PWIDTH (TILE_SIZE * BOARD_W)

# define TILE_SIZE 28U

# define RIGHTBAND_TILE_NB 5U

# define RIGHTBAND_WIDTH (TILE_SIZE * RIGHTBAND_TILE_NB)

# define SCREEN_WIDTH (uint32_t)(TILE_SIZE * BOARD_W)
// # define SCREEN_HEIGHT (uint32_t)(TILE_SIZE * BOARD_H) + TOPBAND_HEIGHT
# define SCREEN_HEIGHT (uint32_t)(TILE_SIZE * BOARD_H)


typedef struct s_teamcolor {
	char		*name;
	int			color;
} t_teamcolor;

typedef struct s_team {
	uint32_t	tid;		/* Team Id */
	uint32_t	tsize;		/* Team Size */
	t_teamcolor	data;		/* Team color ? */
} t_team;

typedef struct s_game
{
	t_xvar		*mlx;
	void		*win;
	t_img		img;
	t_ipc		*ipc;
	t_list		*team;
	uint32_t	team_nb;
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

t_teamcolor	get_new_color(uint32_t current_team_nb, uint32_t team_id);


#endif /* DISPLAY_LEMIPC_H */ 