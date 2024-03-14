# include "../include/lem_ipc.h"

/**
 *  @brief Check if team exist
 *  @param team linked list of team
 *  @param team_id team id
 *  @return 1 if team exist 0 otherwise
*/
int8_t team_exist(t_list **team, uint32_t team_id)
{
    t_list *current = *team;

    while (current != NULL) {
        if (((t_team *)current->content)->tid == team_id) {
            return (1);
        }
        current = current->next;
    }
    return (0);
}

/**
 *  @brief Add team to the list
 *  @param team linked list of team
 *  @param team_id team id
 *  @return 1 if team added 0 otherwise
*/
int8_t add_team(t_list **team, uint32_t team_id)
{
    t_team *new_team = malloc(sizeof(t_team));

    if (new_team == NULL) {
        return (0);
    }
    new_team->tid = team_id;
    new_team->tsize = 1;
    new_team->tcolor = 0;
    ft_lstadd_back(team, ft_lstnew(new_team));
    return (1);
}

/* team compare fun for remove if */
static int team_cmp(void *node)
{
    return (((t_team *)node)->tid == 0);
}

/**
 * @brief Remove team from the list
 * @param team linked list of team
 * @param team_id team id
*/
void remove_team(t_list **team)
{
    ft_lst_remove_if(team, free, team_cmp);
}


void display_team_lst(t_list *team)
{
    t_list *current = team;
    int lstid = 0;

    while (current != NULL) {
        ft_printf_fd(1, CYAN"team_id: %d lst pos |%d|\n"RESET, ((t_team *)current->content)->tid, lstid);
        current = current->next;
        ++lstid;
    }
}

void team_handling(uint32_t *array, uint32_t team_id, int8_t add)
{
	uint32_t i = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		if (array[i] == team_id) {
            return ;
		}
	}
    if (add) {
        array[TEAM_NB] += 1U;
        return ;
    }
    if (array[TEAM_NB] > 0U) {
        array[TEAM_NB] -= 1U;
    }
}


int8_t build_list_number_team(t_player *player ,uint32_t *array)
{
	uint32_t i = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		if (array[i] != 0 && !team_exist(&player->team, array[i])) {
            if (!add_team(&player->team, array[i])) {
                ft_printf_fd(2, "Malloc err build team lst\n");
                return (0);
            }
		}
	}
    if (!team_exist(&player->team, player->team_id)) {
        if (!add_team(&player->team, player->team_id)) {
            ft_printf_fd(2, "Malloc err build team lst\n");
            return (0);
        }
    }
	return (1);
}
