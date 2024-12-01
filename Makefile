include rsc/mk/color.mk
include rsc/mk/source.mk

DISPLAY_NAME	=	lemipc_display
NAME			=	lemipc
CC				=	clang

ASCII_ART		=	./rsc/mk/ascii.sh
ASCII_NAME		=	${NAME}
IPCS_FREE		=	./rsc/sh/check_ipcs_free.sh

LEMIPC_RUN		=	./rsc/sh/run_lemipc.sh
DISPLAY_TEST	=	./rsc/sh/display_test.sh

# MLX_FLAG 		=	-Lmini_mlx -lmlx -lX11 -lXext -lm
# MLX = mini_mlx/libmlx.a


SDL_FLAG	= -L./rsc/lib/install/lib -rpath ./rsc/lib/install/lib -lSDL2 -lSDL2_ttf


ARGS = $(filter-out $@,$(MAKECMDGOALS))

all:		$(NAME)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJ_DIR) $(OBJS) $(DISPLAY_NAME) $(LIST) $(LIBFT)
	@printf "$(CYAN)Compiling ${NAME} with $(CFLAGS) ...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(LIST)
	@printf "$(GREEN)Compiling $(NAME) done$(RESET)\n"

$(DISPLAY_NAME): $(LIBFT) $(LIST) ${MLX} $(DISPLAY_OBJS) 
	@printf "$(CYAN)Compiling ${DISPLAY_NAME} ...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $(DISPLAY_NAME) $(DISPLAY_OBJS) $(LIBFT) $(LIST) ${SDL_FLAG}
# @$(CC) $(CFLAGS) -o $(DISPLAY_NAME) $(DISPLAY_OBJS) $(LIBFT) $(LIST) ${MLX_FLAG} ${SDL_FLAG}
	@printf "$(GREEN)Compiling $(DISPLAY_NAME) done$(RESET)\n"

# ${MLX}:
# ifeq ($(shell [ -f ${MLX} ] && echo 0 || echo 1), 1)
# 	@printf "$(CYAN)Compiling mini_mlx...$(RESET)\n"
# 	@make -s -C mini_mlx
# 	@printf "$(GREEN)Compiling mini_mlx done$(RESET)\n"
# endif

$(LIST):
ifeq ($(shell [ -f ${LIST} ] && echo 0 || echo 1), 1)
	@printf "$(CYAN)Compiling list...$(RESET)\n"
	@$(MAKE_LIST)
	@printf "$(GREEN)Compiling list done$(RESET)\n"
endif

$(LIBFT):
ifeq ($(shell [ -f ${LIBFT} ] && echo 0 || echo 1), 1)
	@printf "$(CYAN)Compiling libft...$(RESET)\n"
	@$(MAKE_LIBFT)
	@printf "$(GREEN)Compiling libft done$(RESET)\n"
endif

$(OBJ_DIR):
	@mkdir $(OBJ_DIR) $(DISPLAY_OBJ_DIR)
	@$(ASCII_ART) $(ASCII_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "$(YELLOW)Compiling $<$(RESET)\n"
	@$(CC) -o $@ -c $< $(CFLAGS)

litle: ${NAME}

verry_litle: ${NAME}

clean:
ifeq ($(shell [ -d ${OBJ_DIR} ] && echo 0 || echo 1), 0)
	@$(RM) $(OBJ_DIR)
	@printf "$(RED)Clean $(OBJ_DIR)/test output$(RESET)\n"
	@$(RM) ${TESTER_OUT_FILES}
endif

fclean:		clean clear clean_lib
	@printf "$(RED)Clean $(NAME)/lib$(RESET)\n"
	@$(RM) $(NAME) ${TESTER_OUT_FILES} ${DISPLAY_NAME}

clean_lib:
	@printf "$(RED)Clean libft$(RESET)\n"
	@$(MAKE_LIBFT) fclean
	@printf "$(RED)Clean list$(RESET)\n"
	@$(MAKE_LIST) fclean
	@printf "$(RED)Clean mini_mlx$(RESET)\n"
	@make -s -C mini_mlx clean

clear:
	@$(IPCS_FREE)
	@$(DISPLAY_TEST) rm

run: $(NAME)
	@${LEMIPC_RUN} ${ARGS}

# Avoid take arg as target
%:
	@:

re:			fclean all

.PHONY:		all clean fclean re bonus
