# include "../include/display.h"

# define REMOVE_TEAM 0
# define ADD_TEAM 1
# define UPDATE_KILL 2 


/**
 *  @brief Check if team exist
 *  @param team linked list of team
 *  @param team_id team id
 *  @return 1 if team exist 0 otherwise
*/
static int8_t team_exist(t_list **team, uint32_t team_id)
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

static void increment_team_size(t_list **team, uint32_t team_id)
{
	t_list *current = *team;

	while (current != NULL) {
		if (((t_team *)current->content)->tid == team_id) {
			((t_team *)current->content)->tsize += 1;
			if (((t_team *)current->content)->strsize != NULL) {
                free(((t_team *)current->content)->strsize);
            }
			((t_team *)current->content)->strsize = ft_itoa(((t_team *)current->content)->tsize);
			return ;
		}
		current = current->next;
	}
}

static t_team *build_team_node(uint32_t team_id)
{
    t_team *team = ft_calloc(1, sizeof(t_team));
    
    if (!team) {
        return (NULL);
    }
    team->tid = team_id;
    team->tsize = 1;
    team->strid = ft_itoa(team_id);
    team->strsize = ft_itoa(team->tsize);
    team->kill = 0;
    return (team);
}

static t_team *get_team_node(t_list **team, uint32_t team_id)
{
    t_list *current = *team;

    while (current != NULL) {
        if (((t_team *)current->content)->tid == team_id) {
            return (current->content);
        }
        current = current->next;
    }
    return (NULL);
}

static void free_team(void *team)
{
	t_team *tmp = (t_team *)team;
    
    if (tmp->strid) {
        free(tmp->strid);
    }
    if (tmp->strsize) {
        free(tmp->strsize);
    }
    if (team) {
    	free(team);
    }
}

void team_handling(t_list **lst, uint32_t *array, uint32_t team_id, int8_t cmd)
{
    t_team      *team = NULL;
	uint32_t    i = 0;
    int8_t      inc_teamsize = team_exist(lst, team_id);

	for (i = 0; i < BOARD_SIZE; i++) {
		if (array[i] == team_id) {
            return ;
		}
	}
    if (cmd == ADD_TEAM) {
        if (inc_teamsize) {
            increment_team_size(lst, team_id);
            return ;
        }
        team = build_team_node(team_id);
        if (team) {
            ft_lstadd_back(lst, ft_lstnew(team));
        }
        return ;
    } 
    
    team = get_team_node(lst, team_id);
    if (cmd == REMOVE_TEAM && team) {
        ft_lst_remove_if(lst, team, free_team, is_same_node);
    } else if (cmd == UPDATE_KILL && team) {
        team->kill++;
    }
}

// int get_min_id(void *next, void *current)
// {
// 	return (((t_team *)next)->tid >= ((t_team *)current)->tid);
// }
// list_sort(lst, get_min_id);
