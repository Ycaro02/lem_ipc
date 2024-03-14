# ifndef DISPLAY_LEMIPC_H
# define DISPLAY_LEMIPC_H

# include "lem_ipc.h"

# define TILE_SIZE 18

# define SCREEN_WIDTH TILE_SIZE * BOARD_W
# define SCREEN_HEIGHT TILE_SIZE * BOARD_H

typedef struct s_game
{
	t_xvar		*mlx;
	void		*win;
	t_img		img;
	t_ipc		*ipc;
}	t_game;

/* global var to handle display with mlx*/
extern t_game *g_game;

typedef enum e_key
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
}	t_key;

#endif /* DISPLAY_LEMIPC_H */ 