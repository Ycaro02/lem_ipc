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
			ft_printf_fd(2, "Error reading from /dev/urandom\n");
			close(fd);
		}
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
	u32 y = idx / BOARD_W;
	u32 x = idx % BOARD_W;

	t_vec pos = create_vector(y, x);
    return (pos);
}

/**
* Get random reachable board position
*/
t_vec get_random_point(u32 *array)
{
    t_vec board_size = create_vector(BOARD_H, BOARD_W);
    t_vec new_point = generate_random_board_point(BOARD_H * BOARD_W);
    int found = -1;
    u32 max_iter = (BOARD_H * BOARD_W);
	u32 nb_iter = 0;
    while (found != 0 && nb_iter < max_iter)
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
        nb_iter++;
    }
	if (nb_iter == max_iter) {
		return (create_vector(UINT32_MAX, UINT32_MAX));
	}
    return (new_point);
}
