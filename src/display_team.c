# include "../include/display.h"

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
	uint32_t lst_size = 0;
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


void increment_team_size(t_list **team, uint32_t team_id)
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

/* list sort function */
void    list_sort(t_list **lst, int (*cmp)())
{
	t_list  *next;
	t_list  *current;

	if (cmp == NULL || lst == NULL || *lst == NULL)
		return ;
	current = *lst;
	next = current;
	while (next && current && current->next)
	{
		next = current->next;
		if (cmp(next->content, current->content) <= 0) { /* if next value lower than current value reverse for strcmp works */

			void *tmp_data = current->content; /* just swap data using tmp */
			current->content = next->content;
			next->content = tmp_data;
			current = *lst;         /* reset current and next */
			continue ;
		}
		current = next;
	}
}

int8_t build_list_number_team(t_list **lst, uint32_t *array)
{
	uint32_t i = 0;
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
