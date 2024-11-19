NAME = ircserv
FLAGS = -g -Wall -Werror -Wextra -std=c++98 -g
CC = c++
RM = rm -rf

SRC_MAIN = main.cpp
SRC_CHANNEL = $(addprefix ./channel/, Channel.cpp)
SRC_CLIENT = $(addprefix ./client/, Client.cpp ) 
SRC_COMMANDS = $(addprefix ./commands/, Command.cpp Authorisation.cpp)
SRC_SERVER = $(addprefix ./server/, Server.cpp)

#Add header files here so they also re-compile
INC = ./channel/Channel.hpp ./client/Client.hpp ./commands/Command.hpp ./server/Server.hpp

SRC = $(SRC_MAIN) $(SRC_SERVER) $(SRC_CHANNEL) $(SRC_CLIENT) $(SRC_COMMANDS)
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp $(INC)
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
