# include "../../include/display.h"

/**
 * @brief Handle team size update digit and string
 * @param team The team to update
 * @param add 1 to add, 0 to remove
*/
static void handle_team_size(t_team *team, s8 add)
{
	if (add) {
		team->tsize += 1;
	} else if (!add && team->tsize > 0) {
		team->tsize -= 1;
	}
	if (team->strsize != NULL) {
		free(team->strsize);
	}
	team->strsize = ft_itoa(team->tsize);
}

/**
 * @brief Increment team kill
 * @param node The node to check
 * @return 1 if the node is the same, otherwise 0
*/
static void increment_team_kill(void *team_node){
	t_team *team = (t_team *)team_node;

	team->kill += 1;
	if (team->kill_str != NULL) {
		free(team->kill_str);
	}
	team->kill_str = ft_itoa(team->kill);
}

/**
 * @brief Build team node
 * @param team_id The team id
 * @return The allocated team node
*/
static t_team *build_team_node(u32 team_id) {
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

/**
 * @brief Get team node
 * @param team The team list
 * @param team_id The team id
 * @return The team node
*/
static t_team *get_team_node(t_list **team, u32 team_id) {
    t_list *current = *team;

    while (current != NULL) {
        if (((t_team *)current->content)->tid == team_id) {
            return (current->content);
        }
        current = current->next;
    }
    return (NULL);
}

/**
 * @brief Free team node
 * @param team The team node to free
*/
void free_team(void *team) {
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

void team_handling(t_game *game, u32 team_id, s8 cmd) {
    t_team      *team = NULL;

    team = get_team_node(&game->team_data, team_id);
    if (cmd == JOIN_TEAM) {
        if (team) {
            handle_team_size(team , 1);
            return ;
        }
        team = build_team_node(team_id);
        if (team) {
            ft_lstadd_back(&game->team_data, ft_lstnew(team));
        }
        return ;
    } 
    
    if (team && cmd == REMOVE_TEAM) {
        handle_team_size(team, 0);
        if (team->tsize == 0) {
			game->kill_from_remove_team += team->kill;
            ft_lst_remove_if(&game->team_data, team, free_team, is_same_node);
        }
    } else if (team && cmd == UPDATE_KILL) {
		increment_team_kill(team);
    }

	list_sort(&game->team_data, get_max_kill);
}


