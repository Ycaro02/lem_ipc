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


void *get_lstteam_head(void *ptr)
{
	return (ptr + TEAM_LST_OFF);
}

int8_t team_handling(void *ptr, uint32_t team_id)
{
    void *team = get_lstteam_head(ptr);
    if (!team_exist((t_list **)team, team_id)) {
        if (!add_team((t_list **)team, team_id)) {
            return (0);
        }
    }
    ft_printf_fd(1, "*team = %p\n", team);
    // display_team_lst(*team);
    return (1);
}

