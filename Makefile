NAME = ./ircserv
CPP = /bin/c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS = main.cpp src/server.cpp
OBJ_DIR = ./obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

# Ensure the subdirectory of OBJ_DIR exists before compiling
$(OBJ_DIR)/%.o: %.cpp | create_obj_dirs
	$(CPP) -c $< -o $@ $(CPPFLAGS)

# Create required directories
create_obj_dirs:
	mkdir -p $(OBJ_DIR)/src

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re create_obj_dirs
