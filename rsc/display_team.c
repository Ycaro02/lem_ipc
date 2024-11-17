# include "../../include/display.h"


// /* @brief Get maximum size in team lst, maybe to remove */
// static u32	get_max_strsize(t_list *list)
// {
// 	t_list		*tmp = list;
// 	u32	max = 0;
// 	u32	tmp_val = 0;

// 	while (tmp) {
// 		tmp_val = ft_strlen(((t_team *)tmp->content)->strsize);
// 		if (tmp_val > max) {
// 			max = tmp_val;
// 		}
// 		tmp = tmp->next;
// 	}
// 	return (max);
// }



/**
 *  @brief Add team to the list
 *  @param team linked list of team
 *  @param team_id team id
 *  @return 1 if team added 0 otherwise
*/
s8 add_team(t_list **team, u32 team_id)
{
    t_team *new_team = malloc(sizeof(t_team));

    if (new_team == NULL) {
        return (0);
    }
	u32 lst_size = 0;
	if (team && *team) {
		lst_size = ft_lstsize(*team);
	}
    new_team->tid	= team_id;
    new_team->tsize	= 0;
    new_team->data	= get_new_color(lst_size, team_id);

	new_team->strid = ft_itoa(team_id);
    new_team->strsize = ft_itoa(1);
	ft_lstadd_back(team, ft_lstnew(new_team));
    return (1);
}


void increment_team_size(t_list **team, u32 team_id)
{
	t_list *current = *team;

	while (current != NULL) {
		if (((t_team *)current->content)->tid == team_id) {
			((t_team *)current->content)->tsize += 1;
			free(((t_team *)current->content)->strsize);
			((t_team *)current->content)->strsize = ft_itoa(((t_team *)current->content)->tsize);
			return ;
		}
		current = current->next;
	}
}


void free_team(void *team)
{
	t_team *tmp = (t_team *)team;
	free(tmp->strid);
	free(tmp->strsize);
	free(tmp);
}

int get_min_id(void *next, void *current)
{
	return (((t_team *)next)->tid >= ((t_team *)current)->tid);
}


s8 build_list_number_team(t_list **lst, u32 *array)
{
	u32 i = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		if (array[i] != TILE_EMPTY)  {
			if (!team_exist(lst, array[i])) {
				if (!add_team(lst, array[i])) {
					ft_printf_fd(2, "Malloc err build team lst\n");
					return (0);
				}
			}
			increment_team_size(lst, array[i]);
		}
	}
	list_sort(lst, get_min_id);
	return (1);
}