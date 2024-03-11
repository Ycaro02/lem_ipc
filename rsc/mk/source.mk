OBJ_DIR			= obj

SRC_DIR 		= src

MAIN_MANDATORY	=	main.c

SRCS			=	start_ipc.c\
					handle_board.c\

SRCS_BONUS		=	main_bonus.c\

MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

OBJS_BONUS		= $(addprefix $(OBJ_DIR)/, $(SRCS_BONUS:.c=.o))

OBJS_MAIN_BONUS	= $(addprefix $(OBJ_DIR)/, $(MAIN_BONUS:.c=.o))

RUN_TESTER	= ./rsc/sh/run_tester.sh

TESTER_OUT_FILES =	nm_out\
					out\
					valgrind_out\
					tester_logs\

RM			=	rm -rf
