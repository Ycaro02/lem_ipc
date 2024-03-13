# include "../include/lem_ipc.h"

/**
* Get random number % max
*/
uint32_t     gener_number(int max)
{
    time_t  t;
    int             nb;

    if (max == 0)
        return (0);
    if (time(&t) == (time_t)-1) {
        printf("Funtion time faillure\n");
        return (-1);
    }
    srand((unsigned)t);
    nb = rand() % max;
    return (nb);
}


/**
  * Get random board position
 */
t_vec generate_random_board_point(uint32_t max)
{
    uint32_t idx = gener_number(max);
	// int idx = (board_size.y * BOARD_W) +  board_size.x;
	uint32_t y = idx / BOARD_W;
	uint32_t x = idx % BOARD_W;

	// ft_printf_fd(1, "idx = %u max= %u\n", idx, max);
	// ft_printf_fd(1, "y = %u, x = %u\n", y, x);
	t_vec pos = create_vector(y, x);
	// ft_printf_fd(1, CYAN"pos = [%u][%u]\n"RESET, pos.y, pos.x);
    return (pos);
}

/**
* Get random reachable board position
*/
t_vec get_reachable_point(uint32_t *array)
{
    t_vec board_size = create_vector(BOARD_H, BOARD_W);
    t_vec new_point = generate_random_board_point(BOARD_H * BOARD_W);
    int found = -1;
    int max_iter = 0;
    while (found != 0 && max_iter < 100)
    {
        if (get_tile_board_val(array, new_point) != 0) { /* ugly brut force case */
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
