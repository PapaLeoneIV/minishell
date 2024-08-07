# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rileone <rileone@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/21 14:02:36 by rileone           #+#    #+#              #
#    Updated: 2024/07/17 11:38:05 by rileone          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
LIBFT_DIR = libft
PRINTF_DIR = ./libft/printf
GNL_DIR = ./libft/gnl
SRCDIR = ./src
INCLUDES = ./includes
UTILS_ENV = $(SRCDIR)/env
TOKENIZER = $(PARSER)/tokenizer
SYNTAX_HANDLER = $(PARSER)/syntax_analizer
QUOT_TOKENIZER = $(PARSER)/quoted_tokenizer
VAR_EXPANS = $(PARSER)/var_expansion
REDIRECTION = $(PARSER)/redirection_parsing
PARSER = $(SRCDIR)/parser
READ_SDTIN = $(SRCDIR)/read_stdin
EXECUTOR = $(SRCDIR)/executor
BUILTIN = $(SRCDIR)/builtin
CLEANING = $(SRCDIR)/cleaning
SIGNALS = $(SRCDIR)/signals
HEREDOC_TOKENIZER = $(SRCDIR)/parser/heredoc_tokenizer
EVERY_INCLUDES=-I. -I$(LIBFT_DIR) -I$(PRINTF_DIR) -I$(GNL_DIR) -I$(INCLUDES) -I$(SRCDIR) -I$(UTILS_ENV) -I$(READ_SDTIN)

RM = rm -f
CC = cc
CFLAGS = -Wall -Wextra -Werror -g -I./$(INCLUDES)
COMPILE = $(CC) $(CFLAGS)
ARCHIVE = ar rcs
SRC =			$(addprefix $(UTILS_ENV)/, env_helpers.c env_manipulating.c export_path.c) \
				$(addprefix $(READ_SDTIN)/, read_input.c ) \
				$(addprefix $(TOKENIZER)/, tokenizer_state_methods.c tokenizer_heredoc_helpers.c tokenizer_state_methods_help.c tokenizer.c tokenizer_utils.c tokenizer_helpers.c tokenizer_slice_methods.c tokenizer_slice_helpers.c) \
				$(addprefix $(QUOT_TOKENIZER)/,quot_tokenizer.c quot_tokenizer_utils.c quot_tokenizer_utils2.c unpack_quoted_tokens.c) \
				$(addprefix $(SYNTAX_HANDLER)/, syntax_analizer.c syntax_analizer_helpers.c syntax_extra_utils.c) \
				$(addprefix $(VAR_EXPANS)/, var_expansion_helpers.c)  \
				$(addprefix $(EXECUTOR)/, execution_maker.c redir_exe_utils.c exe_utils.c execute_builtin.c here_path.c cat_exeption.c) \
				$(addprefix $(SIGNALS)/, signals_helpers.c) \
				$(addprefix $(HEREDOC_TOKENIZER)/, heredoc_tokenizer.c heredoc_tokenizer_helpers.c) \
				$(addprefix $(BUILTIN)/, base_path.c redline_utils.c exit_build.c cd_path.c)  \
				$(addprefix $(REDIRECTION)/, redirection_parser.c redirection_parser_helpers.c redirection_parser_utils.c)  \
				$(addprefix $(CLEANING)/, tokenizer_cleaner.c env_cleaner.c cmd_cleaner.c)  \
				$(addprefix $(SRCDIR)/, main.c) \

all: $(NAME)


$(NAME): $(SRC)
	@$(MAKE) -C $(LIBFT_DIR)
	@$(COMPILE) $(CFLAGS) $(EVERY_INCLUDES) $(SRC) -L$(LIBFT_DIR) -lft -L$(GNL_DIR) -lgnl -L$(PRINTF_DIR) -lftprintf -lreadline -o $(NAME)
clean:	
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@$(RM) $(NAME)

VALGRIND = valgrind --suppressions=ignore_readline.txt --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes  -s

valgrind: all
	$(VALGRIND) ./$(NAME)

GDB = gdb

gdb: all
	$(GDB) ./$(NAME)


re: fclean all

.PHONY: all clean fclean