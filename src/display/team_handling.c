# include "../../include/display.h"


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

static void handle_team_size(t_list **team, uint32_t team_id, int8_t add)
{
	t_list *current = *team;

	while (current != NULL) {
		if (((t_team *)current->content)->tid == team_id) {
            if (add) {
			    ((t_team *)current->content)->tsize += 1;
            } else if (((t_team *)current->content)->tsize > 0) {
			    ((t_team *)current->content)->tsize -= 1;
            }
			if (((t_team *)current->content)->strsize != NULL) {
                free(((t_team *)current->content)->strsize);
            }
			((t_team *)current->content)->strsize = ft_itoa(((t_team *)current->content)->tsize);
			return ;
		}
		current = current->next;
	}
}


static void handle_team_kill(t_list **team, uint32_t team_id)
{
	t_list *current = *team;

	while (current != NULL) {
		if (((t_team *)current->content)->tid == team_id) {
                ((t_team *)current->content)->kill += 1;
			if (((t_team *)current->content)->kill_str != NULL) {
                free(((t_team *)current->content)->kill_str);
            }
			((t_team *)current->content)->kill_str = ft_itoa(((t_team *)current->content)->kill);
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
	team->kill_str = ft_itoa(team->kill);
	team->color = get_new_color(team_id);
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

void free_team(void *team)
{
	t_team *tmp = (t_team *)team;
    
    if (tmp->strid) {
        free(tmp->strid);
    }
    if (tmp->strsize) {
        free(tmp->strsize);
    }
    if (tmp->kill_str) {
        free(tmp->kill_str);
    }
    if (team) {
    	free(team);
    }
}

static int get_max_kill(void *next, void *current) {
	return (((t_team *)next)->kill <= ((t_team *)current)->kill);
}

void team_handling(t_list **lst, uint32_t *array, uint32_t team_id, int8_t cmd)
{
    t_team      *team = NULL;
    int8_t      inc_teamsize = team_exist(lst, team_id);
    (void)array;

    if (cmd == JOIN_TEAM) {
        if (inc_teamsize) {
            // ft_printf_fd(2, "Team %u exist\n", team_id);
            handle_team_size(lst, team_id, 1);
            return ;
        }
        // ft_printf_fd(2, "Team %u does not exist\n", team_id);
        team = build_team_node(team_id);
        if (team) {
            ft_lstadd_back(lst, ft_lstnew(team));
        }
        return ;
    } 
    
    team = get_team_node(lst, team_id);
    if (cmd == REMOVE_TEAM && team) {
        handle_team_size(lst, team_id, 0);
        team = get_team_node(lst, team_id);
        if (team->tsize == 0) {
            ft_lst_remove_if(lst, team, free_team, is_same_node);
        }
    } else if (cmd == UPDATE_KILL && team) {
        handle_team_kill(lst, team_id);
        /* get team killer node and increment this->kill */
        // team->kill++;
    }
	list_sort(lst, get_max_kill);

}


