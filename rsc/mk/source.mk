OBJ_DIR			= obj

SRC_DIR 		= src

MAIN_MANDATORY	=	main.c

MAIN_DISPLAY	=	src/main_display.c

SRCS			=	init_shared_mem.c\
					handle_board.c\
					init_semaphores.c\

DISPLAY_SRCS	= 	src/main_display.c\
					src/init_shared_mem.c\
					src/handle_board.c\
					src/init_semaphores.c\

# SRCS_BONUS		=	main_bonus.c\

MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

DISPLAY_OBJS	= $(addprefix $(OBJ_DIR)/, $(DISPLAY_SRCS:.c=.o))


# OBJS_BONUS		= $(addprefix $(OBJ_DIR)/, $(SRCS_BONUS:.c=.o))

# OBJS_MAIN_BONUS	= $(addprefix $(OBJ_DIR)/, $(MAIN_BONUS:.c=.o))

RUN_TESTER	= ./rsc/sh/run_tester.sh

TESTER_OUT_FILES =	nm_out\
					out\
					valgrind_out\
					tester_logs\

RM			=	rm -rf
