NAME = ircserv
FLAGS = -Wall -Werror -Wextra -std=c++98 -g
CC = c++
RM = rm -rf

SRC_MAIN = main.cpp
SRC_CHANNEL = $(addprefix ./channel/, )
SRC_CLIENT = $(addprefix ./client/, ) 
SRC_COMMANDS = $(addprefix ./commands/, )
SRC_SERVER = $(addprefix ./server/, )


SRC = $(SRC_MAIN) $(SRC_CHANNEL) $(SRC_CLIENT) $(SRC_COMMANDS) $(SRC_SERVER)
OBJ = $(SRC:.cpp=.o)


all: $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
