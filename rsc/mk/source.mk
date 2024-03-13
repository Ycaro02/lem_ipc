OBJ_DIR			= obj

SRC_DIR 		= src

MAIN_MANDATORY	=	main.c

MAIN_DISPLAY	=	main_display.c

SHARED_SRCS		=	init_shared_mem.c\
					handle_board.c\
					init_semaphores.c\
					random_position.c\

DISPLAY_SRCS	= 	${SHARED_SRCS}\
					${MAIN_DISPLAY}

SRCS			=	${SHARED_SRCS}

MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

DISPLAY_OBJS	= $(addprefix $(OBJ_DIR)/, $(DISPLAY_SRCS:.c=.o))

IPCS_FREE		= ./rsc/sh/check_ipcs_free.sh

LEMIPC_RUN		= ./rsc/sh/run_lemipc.sh

# SRCS_BONUS		=	main_bonus.c

# OBJS_BONUS		= $(addprefix $(OBJ_DIR)/, $(SRCS_BONUS:.c=.o))

# OBJS_MAIN_BONUS	= $(addprefix $(OBJ_DIR)/, $(MAIN_BONUS:.c=.o))

RUN_TESTER	= ./rsc/sh/run_tester.sh

RM			=	rm -rf

ifeq ($(findstring bonus, $(MAKECMDGOALS)), bonus)
ASCII_NAME	= "bonus"
SRCS += $(SRCS_BONUS)
else
SRCS += $(MAIN_MANDATORY)
endif
