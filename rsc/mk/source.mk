OBJ_DIR			= obj

DISPLAY_OBJ_DIR	= obj/display

SRC_DIR 		= src

MAIN_MANDATORY	=	main.c

MAIN_DISPLAY	=	display/main_display.c

SHARED_SRCS		=	init_shared_mem.c\
					init_game.c\
					handle_board.c\
					sem_handling.c\
					random_position.c\
					ipcs_utils.c\
					player.c\
					player_move.c\
					send_pdata.c\


DISPLAY_SRCS	= 	${SHARED_SRCS}\
					${MAIN_DISPLAY}\
					display/click_event.c\
					display/parse_pdata.c\
					display/display_pdata.c\
					display/team_handling.c\


SRCS			=	${SHARED_SRCS}

MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

DISPLAY_OBJS	= $(addprefix $(OBJ_DIR)/, $(DISPLAY_SRCS:.c=.o))

RUN_TESTER	= ./rsc/sh/run_tester.sh

RM			=	rm -rf

SRCS += $(MAIN_MANDATORY)
