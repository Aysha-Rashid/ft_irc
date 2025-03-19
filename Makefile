NAME = ircserv

SRC = main.cpp Server.cpp Client.cpp Channel.cpp Utils.cpp nick.cpp join.cpp
	 	 
OBJ = $(SRC:.cpp=.o)

CXX = c++

CFLAGS = -std=c++98 -Wall -Wextra -Werror

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
