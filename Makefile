include rsc/mk/color.mk
include rsc/mk/source.mk

DISPLAY_NAME	=	lemipc_display
NAME			=	lemipc
CC				=	gcc
CFLAGS			=	-Wall -Wextra -Werror -O3 -g -D _GNU_SOURCE -D __USE_GNU

ASCII_ART		=	./rsc/mk/ascii.sh
ASCII_NAME		=	${NAME}
IPCS_FREE		=	./rsc/sh/check_ipcs_free.sh

LEMIPC_DOWN		= 	./rsc/sh/handle_sigint.sh
LEMIPC_RUN		=	./rsc/sh/run_lemipc.sh
DISPLAY_TEST	=	./rsc/sh/display_test.sh
MLX_FLAG 		=	-Lminilibx-linux -lmlx -lX11 -lXext -lm

all:		$(NAME)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJ_DIR) $(OBJS) $(DISPLAY_NAME) $(LIST) $(LIBFT)
	@printf "$(CYAN)Compiling ${NAME} ...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(LIST)
	@printf "$(GREEN)Compiling $(NAME) done$(RESET)\n"

$(DISPLAY_NAME): $(DISPLAY_OBJS) $(LIST) $(LIBFT)
	@make -s -C minilibx-linux
	@printf "$(CYAN)Compiling ${DISPLAY_NAME} ...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $(DISPLAY_NAME) $(DISPLAY_OBJS) $(LIBFT) $(LIST) ${MLX_FLAG}
	@printf "$(GREEN)Compiling $(DISPLAY_NAME) done$(RESET)\n"

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
	@mkdir $(OBJ_DIR)
	@$(ASCII_ART) $(ASCII_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "$(YELLOW)Compile $<$(RESET)\n"
	@$(CC) -o $@ -c $< $(CFLAGS)

bonus: clear_mandatory ${NAME}

clear_mandatory:
ifeq ($(shell [ -f ${OBJ_DIR}/main.o ] && echo 0 || echo 1), 0)
	@printf "$(RED)Clean mandatory obj $(RESET)\n"
	@rm -rf ${OBJ_DIR}
endif

clean:
ifeq ($(shell [ -d ${OBJ_DIR} ] && echo 0 || echo 1), 0)
	@$(RM) $(OBJ_DIR)
	@printf "$(RED)Clean $(OBJ_DIR)/test output$(RESET)\n"
	@$(RM) ${TESTER_OUT_FILES}
endif

fclean:		clean clear
	@printf "$(RED)Clean $(NAME)/lib$(RESET)\n"
	@$(RM) $(NAME) ${TESTER_OUT_FILES} ${DISPLAY_NAME}

clean_lib:
	@$(MAKE_LIBFT) fclean
	@$(MAKE_LIST) fclean
	@make -C minilibx-linux clean

test: $(NAME)
	@./$(NAME) 1 && ${CHECK_IPC}

display: $(NAME)
	@$(DISPLAY_TEST)

clear:
	@$(LEMIPC_DOWN)
	@$(DISPLAY_TEST) rm
	@$(IPCS_FREE)

run: $(NAME)
	@$(LEMIPC_RUN)

re:			fclean all

.PHONY:		all clean fclean re bonus
