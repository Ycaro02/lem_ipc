# include "../include/display.h"


t_vec get_click_tile(t_vec mouse)
{
    uint32_t y = mouse.y / TILE_SIZE;
    uint32_t x = mouse.x / TILE_SIZE;
	if (x >= BOARD_W || y >= BOARD_H)
		return (create_vector(UINT32_MAX, UINT32_MAX));
	// ft_printf_fd(2, "Mouse y: %d x: %d\n", mouse.y, mouse.x);
	// ft_printf_fd(2, "Vec y: %d x: %d\n", y, x);
	return (create_vector(y, x));
}
