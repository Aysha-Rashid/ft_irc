NAME = ircserv

SRC = main.cpp Server.cpp Client.cpp Channel.cpp Utils.cpp nick.cpp join.cpp mode.cpp topic.cpp
	 	 
OBJ = $(SRC:.cpp=.o)

CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
