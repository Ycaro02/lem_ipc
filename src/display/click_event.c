# include "../../include/display.h"


t_vec get_click_tile(t_vec mouse)
{
    u32 y = mouse.y / TILE_SIZE;
    u32 x = mouse.x / TILE_SIZE;
	if (x >= BOARD_W) {
		return (create_vector(mouse.y / (TILE_SIZE * 2), UINT32_MAX));
	}
	return (create_vector(y, x));
}

/* @brief check left mouse click, update game->mouse_pos in consequences */
int check_mouse(int keycode, int x, int y, Game *game)
{
	if (keycode == LEFT_CLICK) {
		game->mouse_pos = get_click_tile(create_vector(y, x));
		return (1);
	}
	return (0);
}
