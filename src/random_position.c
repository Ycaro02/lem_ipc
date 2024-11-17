# include "../include/lem_ipc.h"

/**
* Get random number % max
*/
u32     gener_number(int max)
{
	u32 nb = 0;

	int fd = open("/dev/urandom", O_RDONLY); 
	if (fd != -1)
	{
		char buff[4] = {0};
		int len = read(fd, &buff, 4);
		if (len == -1) {
			printf("Error reading from /dev/urandom\n");
			close(fd);
		}
		// ft_printf_fd(1, YELLOW"RANDOM buff = |%d|%d|%d|%d|\n"RESET, buff[0], buff[1], buff[2], buff[3]);
		nb = (*(u32 *)buff) % max;
		close(fd);
	}
    return (nb);
}


/**
  * Get random board position
 */
t_vec generate_random_board_point(u32 max)
{
    u32 idx = gener_number(max);
	// ft_printf_fd(1, CYAN"idx generated: %d\n"RESET, idx);
	// int idx = (board_size.y * BOARD_W) +  board_size.x;
	u32 y = idx / BOARD_W;
	u32 x = idx % BOARD_W;

	t_vec pos = create_vector(y, x);
    return (pos);
}

/**
* Get random reachable board position
*/
t_vec get_random_point(u32 *array, t_vec player_pos)
{
    t_vec board_size = create_vector(BOARD_H, BOARD_W);
    t_vec new_point = generate_random_board_point(BOARD_H * BOARD_W);
    int found = -1;
    int max_iter = 0;
	(void)player_pos;
	// ft_printf_fd(1, "player_pos: [%d][%d]\n", player_pos.y, player_pos.x);
	// ft_printf_fd(1, "new_point: [%d][%d]\n", new_point.y, new_point.x);
	// if (get_tile_board_val(array, new_point) != 0) {
	// 	return (player_pos);
	// }
    while (found != 0 && max_iter < 100)
    {
        if (get_tile_board_val(array, new_point) != TILE_EMPTY) { /* brut force case */
            if (new_point.x < board_size.x - 1)
                new_point.x++;
            else {
                new_point.x = 0;
                new_point.y++;
                if (new_point.y >= board_size.y)
                    new_point.y = 0;
            }
        } else {
            found = 0;
		}
        max_iter++;
    }
    // printf("%snew_point: [%d][%d]%s\n",GREEN ,new_point.y, new_point.x, RESET);
    return (new_point);
}
