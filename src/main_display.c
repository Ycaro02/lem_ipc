# include "../include/display.h"

int		g_game_run; 

t_teamcolor get_new_color(uint32_t current_team_nb, uint32_t team_id) 
{
	t_teamcolor team_color[] = {
		{"Red", 0xFF0000},
		{"Blue", 0x6699FF},
		{"Yellow", 0xFFCC00},
		{"Green", 0x00FF00},
		{"Pink", 0xFF00CC},
		{"Purple", 0xCC33FF},
		{"Cyan", 0x00FFFF},
		{"Orange", 0xFF9900},
		{"Brown", 0x99991E},
		{"Grey", 0x999999},
		{"Red Light", 0xCC0000},
		{"Pink Light", 0xFFCCCC},
		{"Yellow Light", 0xFFFF00},
		{"Green Light", 0xCCFF00},
		{"Green Dark", 0x358000},
		{"Blue Light", 0x0000CC},
		{"Blue Light", 0x99CCFF},
		{"Cyan Light", 0xCCFFFF},
		{"Purple Light", 0x9900CC},
		{"Purple Light", 0xCC99FF},
		{"Brown Light", 0x996600},
		{"Brown Dark", 0x666600},
		{"Grey Dark", 0x666666},
		{"Grey Light", 0xCCCCCC},
		{"Green Light", 0x79CC3D},
		{"Grey Light", 0xCCCC99},
	};


	(void)current_team_nb;
	uint32_t idx = 0;
	for (uint32_t i = 0; COLOR_MAX ; i++) {
			if (i == (team_id - 1) % COLOR_MAX) {
				idx = i;
				break ;
			}
		}
	// ft_printf_fd(2, CYAN"idx %d\n"RESET, idx);
	return (team_color[idx]);
}


/* @brief Initialize display */
int init_display(t_player *player, int argc, char **argv)
{
	if (argc != 1) {
		ft_printf_fd(2, "Usage: no args required %s\n", argv[0]);
		return (-1);
	}
	player->team_id = UINT32_MAX;
	return (0);
}



static void display_pdata_lst(t_list *player_lst)
{
	t_list *tmp = player_lst;
	t_pdata *pdata = NULL;
	while (tmp) {
		pdata = tmp->content;
		for (int i = 1; i < PDATA_LEN; i++) {
			ft_printf_fd(2, YELLOW"%s: "RESET, pdata[i].name);
			if (i >= PDATA_POS) {
				ft_printf_fd(2, CYAN"[%u] [%u]\n"RESET, pdata[i].vdata.y, pdata[i].vdata.x);
			} else {
				ft_printf_fd(2, PURPLE"%u\n"RESET, pdata[i].sdata);
			}
		}
		tmp = tmp->next;
	}
}


static void *get_player_node(t_list *lst, t_vec target)
{
	for (t_list *current = lst; current; current = current->next) {
		ft_printf_fd(2, RED"data:vec  %u %u", ((t_pdata *)current->content)[PDATA_POS].vdata.y, ((t_pdata *)current->content)[PDATA_POS].vdata.x);
		if (vector_cmp(((t_pdata *)current->content)[PDATA_POS].vdata, target)) {
			return (current->content);
		}
	}
	return (NULL);
}

static int is_same_node(void *node, void *target)
{
	return (node == target);
}

static void receive_player_data(t_game *game)
{
	t_pdata		pdata[PDATA_LEN] = INIT_MSG_PACK;
	uint32_t	ret = UINT32_MAX;
	uint8_t		count = 1; /* count = 1 cause if we call this we already receive the first 0 data start */

	do {
		ret = extract_msg(game->ipc, UINT32_MAX);
		pdata[count].sdata = ret;
		if (count != PDATA_START) {
			if (count >= PDATA_POS) {
				pdata[count].vdata = get_board_pos(pdata[count].sdata);
			} 
		}
		++count;
		if (count >= PDATA_LEN) {
			uint32_t type_state = pdata[PDATA_STATE].sdata;
			uint8_t type = GET_MSG_TYPE(type_state);
			uint8_t state = GET_MSG_STATE(type_state);
			ft_printf_fd(2, YELLOW"Player state %u, type: %d, state, %d\n"RESET, type_state, type, state);
			if (type == P_CREATE) {
				ft_printf_fd(2, CYAN"Player data create\n"RESET);
				t_pdata *tmp = ft_calloc(sizeof(t_pdata), PDATA_LEN);
				ft_memcpy((void *)tmp, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
				ft_lstadd_back(&game->player_data, ft_lstnew(tmp));
			} else {
				t_pdata *target_node = get_player_node(game->player_data, pdata[PDATA_POS].vdata);
				if (!target_node) {
					ft_printf_fd(2, RED"Error: player node not found in P_DELETE/UPDATE case\n"RESET);
					return ;
				}
				if (type == P_DELETE) {
					ft_printf_fd(2, RED"Player data delete\n"RESET);
					ft_lst_remove_if(&game->player_data, target_node, free, is_same_node);
				} else if (type == P_UPDATE) {
					pdata[PDATA_POS] = pdata[PDATA_TID];		/* update PPOS */
					pdata[PDATA_TID] = target_node[PDATA_TID]; /* restore player TID*/
					ft_printf_fd(2, CYAN"Player data update\n"RESET);
					ft_memcpy((void *)target_node, (void *)pdata, sizeof(t_pdata) * PDATA_LEN);
				}
			} 

			count = PDATA_START;
		}
	} while (ret != UINT32_MAX);
}


int destroy_windows(t_game *game)
{
	mlx_destroy_image(game->mlx, game->img.image);
	mlx_destroy_window(game->mlx, game->win);
	mlx_destroy_display(game->mlx);
	if (game->team) {
		ft_lstclear(&game->team, free_team);
	}
	if (game->player_data) {
		ft_lstclear(&game->player_data, free);
	}
	if (game->pause){
		ft_printf_fd(2, CYAN"Display was in pause state: restore sem\n"RESET);
		ft_printf_fd(1, RED"Exit MLX\n"RESET);
		sem_unlock(game->ipc->semid);
	}
	free(game->mlx);
	free(game);
	exit(0);
}


int display_board_stdout(t_game *game) {
	sem_lock(game->ipc->semid);
	if (get_attached_processnb(game->ipc) <= 1) {
		ft_printf_fd(2, RED"Shutdown display\n"RESET);
		g_game_run = 0;
		detach_shared_memory(game->ipc);
		sem_unlock(game->ipc->semid);
		destroy_windows(game);
	}
	display_uint16_array(game->ipc->ptr);
	sem_unlock(game->ipc->semid);
	usleep(10000); /* 1/10 sec */
	return (0);
}

/* to put in header file */

// void		mystring_put(t_xvar *xvar,t_win_list *win,int x,int y,int color,char *string)
// {
//    XGCValues	xgcv;
   
//    xgcv.foreground = mlx_int_get_good_color(xvar,color);
//    XChangeGC(xvar->display,win->gc,GCForeground,&xgcv);
//    XDrawString(xvar->display,win->window,win->gc,x,y,string,strlen(string));
// }


int skip_x(char *str) {
	return (ft_strlen(str) * CHAR_TOPIXEL_SKIP);
}

static int	display_team_info(t_game *game, t_team *team, uint32_t pad_y, uint32_t str_sizemax)
{
	uint32_t start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U;
	uint32_t y = 20U + pad_y;

	uint32_t x = start_x;

	mlx_string_put(game->mlx, game->win, x, y, team->data.color, "ID : ");
	x += skip_x("ID : ");
	mlx_string_put(game->mlx, game->win, x, y, CYAN_INT, team->strid);
	x += ((str_sizemax + 2) * CHAR_TOPIXEL_SKIP);
	mlx_string_put(game->mlx, game->win, x, y, team->data.color, "REMAIN : ");
	x += skip_x("REMAIN : ");
	mlx_string_put(game->mlx, game->win, x, y, CYAN_INT, team->strsize);
	
	return (0);
}

static uint32_t	get_max_strsize(t_list *list)
{
	t_list		*tmp = list;
	uint32_t	max = 0;
	uint32_t	tmp_val = 0;

	while (tmp) {
		tmp_val = ft_strlen(((t_team *)tmp->content)->strsize);
		if (tmp_val > max) {
			max = tmp_val;
		}
		tmp = tmp->next;
	}
	return (max);
}

void display_button(t_game *game)
{
	uint32_t width = RIGHTBAND_WIDTH;
	uint32_t height = TILE_SIZE * 2;
	uint32_t start_x = SCREEN_WIDTH - RIGHTBAND_WIDTH;

	for (uint32_t y = 0; y < height; ++y) {
		for (uint32_t x = start_x; x < start_x + width; ++x) {
			((uint32_t *)game->img.data)[x + (y * SCREEN_WIDTH)] = BLUE_INT;
		}
	}
}

void display_teamlist(t_game *game, t_list *list)
{
	display_button(game);
	t_list *tmp = list;
	uint32_t y = (TILE_SIZE * 2) + 15U;


	char *player_remain = ft_ultoa(get_attached_processnb(game->ipc) - 1U);
	if (player_remain) {
		uint32_t str_size_max = get_max_strsize(list);
		mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + (TILE_SIZE * 2)), TILE_SIZE, BLACK_INT, "PAUSE");
		// y += PAD_YTEAM;
		mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U), y, CYAN_INT, PLAYER_REMAIN);
		mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U) + skip_x(PLAYER_REMAIN), y, RED_INT, player_remain);
		y += PAD_YTEAM;
		mlx_string_put(game->mlx, game->win, (SCREEN_WIDTH - RIGHTBAND_WIDTH + 5U), y, CYAN_INT, "TEAM INFO : ");
		free(player_remain);

		while (tmp) {
			display_team_info(game, tmp->content, y, str_size_max);
			tmp = tmp->next;
			y += PAD_YTEAM;
		}
	}
}

int	key_hooks_press(int keycode, t_game *game)
{
	if (keycode == ESC)
		destroy_windows(game); /* maybe need to check sem value and lock it to detash mem */
	return (0);
}


static int get_team_color(t_list *team, uint32_t team_id)
{
	t_list *tmp = team;
	while (tmp) {
		if (((t_team *)tmp->content)->tid == team_id) {
			return (((t_team *)tmp->content)->data.color);
		}
		tmp = tmp->next;
	}
	return (0);
}


static void draw_board(t_game *game)
{
	int color = 0;

	for (uint32_t y = 1; y < SCREEN_HEIGHT; ++y) {
		for (uint32_t x = 1; x < SCREEN_WIDTH - RIGHTBAND_WIDTH; ++x) {
			uint32_t x_compute =  ((x / TILE_SIZE) % BOARD_W);
			uint32_t y_compute = ((y / TILE_SIZE) * BOARD_W);   
			uint32_t idx = x_compute + y_compute;
			uint32_t tile_state = game->ipc->ptr[idx];
			color = tile_state == TILE_EMPTY ? 0xFFFFFF : get_team_color(game->team, tile_state);
			if (x % TILE_SIZE != 0 && y % TILE_SIZE != 0) {
				((uint32_t *)game->img.data)[x + (y * SCREEN_WIDTH)] = color;
			}
		}
	}

}

int boardmlx_display(void *vgame)
{
	t_game		*game = vgame;
	uint32_t	tmp = 0U;

	/* If game is paused sem already lock */
	if (!game->pause) {
		sem_lock(game->ipc->semid);
	}
	/* Check for game pause click */
	if (game->mouse_pos.y == 0 && game->mouse_pos.x == UINT32_MAX) {
		game->pause = !(game->pause);
		game->mouse_pos.y = UINT32_MAX;
		ft_printf_fd(2, CYAN"Game pause %d\n"RESET, game->pause);
	}	

	if (!game->pause) {
		/* Update team info lst */
		if ((uint32_t) get_attached_processnb(game->ipc) != game->player_nb) {
			ft_lstclear(&game->team, free_team);
			if (!build_list_number_team(&game->team, game->ipc->ptr)) {
				ft_printf_fd(2, RED"Error: build_list_number_team\n"RESET);
			}
			game->player_nb = get_attached_processnb(game->ipc);
		}
		/* Update player data lst */
		tmp = extract_msg(game->ipc, UINT32_MAX);
		if (tmp != UINT32_MAX) {
			receive_player_data(game);
		}
	}

	/* Check if only one team left or impossible finish (2 player left) + 1 process for display handler */
	if (game->ipc->ptr[TEAM_NB] <= 1 || get_attached_processnb(game->ipc) <= 3) {
		ft_printf_fd(2, PURPLE"Shutdown display team number [NB] won\n"RESET);
		g_game_run = 0;
		detach_shared_memory(game->ipc);
		sem_unlock(game->ipc->semid);
		destroy_windows(game);
	}

	/* Draw board */
	draw_board(game);

	if (!game->pause) {
		sem_unlock(game->ipc->semid);
	}

	mlx_put_image_to_window(game->mlx, game->win, game->img.image, 0, 0);
	if (game->team) {
		display_teamlist(game, game->team);
	}
	if (game->player_data) {
		display_pdata_lst(game->player_data);
		// ft_lstclear(&game->player_data, free);
	}

	usleep(10000); /* 1/10 sec */
	return (0);
}

static int check_mouse(int keycode, int x, int y, t_game *game)
{
	// int y = -1, x = -1;
	if (keycode == LEFT_CLICK) {
		// ft_printf_fd(2, CYAN"Mouse click %d pos [%d][%d] game %p\n"RESET, keycode, y, x, game);
		t_vec mouse = create_vector(y, x);
		t_vec mouse_pos = get_click_tile(mouse);
		if (mouse_pos.x == UINT32_MAX) {
			ft_printf_fd(2, RED"Mouse click on btn number %u\n"RESET, mouse_pos.y);
			game->mouse_pos = create_vector(mouse_pos.y, mouse_pos.x);
		}
		// ft_printf_fd(2, "Mouse UINT32 click y: %d x: %d\n", mouse.y, mouse.x);
		// ft_printf_fd(2, YELLOW"Tile click y: %d x: %d\n"RESET, game->mouse_pos.y, game->mouse_pos.x);
		return (1);
	}
	return (0);
}


int8_t init_mlx(t_game *game) 
{
	int endian = 0;
	game->mlx = mlx_init();
	if (!game->mlx) {
		ft_printf_fd(2, "mlx_init failed\n");
		return (ERROR_CASE);
	}
	game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "LEM_IPC");
	game->img.image = mlx_new_image(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	// ft_printf_fd(1, CYAN"SCREEN WIDTH %u SCREEN HEIGHT %u\n"RESET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!game->win || !game->img.image) {
		ft_printf_fd(2, "mlx_new_window or image failed\n");
		return (ERROR_CASE);
	}
	game->img.data = mlx_get_data_addr(game->img.image, &game->img.bpp,
			&game->img.width, &endian);
	if (!game->img.data) {
		ft_printf_fd(2, "mlx_get_data_addr failed\n");
		return (ERROR_CASE);
	}

	mlx_hook(game->win, 2, 1L, key_hooks_press, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, destroy_windows, game);
	ft_printf_fd(2, "Game ptr before check mouse %p\n", game);
	mlx_mouse_hook(game->win, check_mouse, game);
	// mlx_loop_hook(game->mlx, display_board_stdout, game);
	mlx_loop_hook(game->mlx, boardmlx_display, game);
	mlx_loop(game->mlx);
	return (0);

}

int main(int argc, char **argv) 
{
	t_ipc		ipc = {};
	t_player	player = {};
	t_game		*game = ft_calloc(sizeof(t_game), 1);

	game->ipc = &ipc;
	game->player_nb = 0;

	if (init_display(&player, argc, argv) != 0\
		|| init_game(&ipc, IPC_NAME, DISPLAY_HANDLER) == ERROR_CASE\
		|| init_mlx(game) == ERROR_CASE) {
		return (1);
	}
	return (0);
}


/* CLear pixel buff */
// size_t len = sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT);
// ft_printf_fd(2, RED"len : %u, sizeof %u\n"RESET, len, sizeof(game->img.data));
// ft_bzero(game->img.data, len);