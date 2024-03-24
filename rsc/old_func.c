/* @brief display board in stdout Unused */
// void display_uint16_array(uint32_t *array)
// {
// 	ft_printf_fd(1, "\n\nDisplay board\n");
// 	uint32_t i = 0, j = 0;
// 	(void)array;

// 	for (i = 0; i < BOARD_H; i++) {
// 		for (j = 0; j < BOARD_W; j++) {
// 			uint32_t idx = i * BOARD_W + j;
// 			// ft_printf_fd(2, "For idx = %u, vec = [%u][%u]\n", idx, get_board_vec(idx).y, get_board_vec(idx).x);
// 			ft_printf_fd(1, YELLOW"["RESET""CYAN"%u"RESET""YELLOW"] "RESET, array[idx]);
// 		}
// 		ft_printf_fd(1, "\n");
// 	}
// 	ft_printf_fd(1, "\n");
// }
