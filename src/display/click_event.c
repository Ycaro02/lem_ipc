# include "../../include/display.h"


t_vec get_click_tile(t_vec mouse)
{
    u32 y = mouse.y / TILE_SIZE;
    u32 x = mouse.x / TILE_SIZE;
	if (x >= BOARD_W) {
		return (create_vector(mouse.y / (TILE_SIZE * 2), UINT32_MAX));
	}
	// ft_printf_fd(2, "Mouse y: %d x: %d\n", mouse.y, mouse.x);
	// ft_printf_fd(2, "Vec y: %d x: %d\n", y, x);
	return (create_vector(y, x));
}

/* @brief check left mouse click, update game->mouse_pos in consequences */
int check_mouse(int keycode, int x, int y, t_game *game)
{
	if (keycode == LEFT_CLICK) {
		game->mouse_pos = get_click_tile(create_vector(y, x));
		return (1);
	}
	return (0);
}
