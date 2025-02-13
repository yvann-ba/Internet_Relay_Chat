NAME = ./ircserv
CPP = /bin/c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp src/server.cpp src/client.cpp src/utils.cpp src/channel.cpp \
		src/cmds/passCommand.cpp src/cmds/nickCommand.cpp src/cmds/userCommand.cpp \
		src/cmds/joinCommand.cpp src/cmds/kickCommand.cpp src/cmds/inviteCommand.cpp \
		src/cmds/modeCommand.cpp src/cmds/privmsgCommand.cpp src/cmds/topicCommand.cpp \
		src/cmds/quitCommand.cpp



OBJ_DIR = ./obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp | create_obj_dirs
	$(CPP) -c $< -o $@ $(CPPFLAGS)

create_obj_dirs:
	mkdir -p $(OBJ_DIR)/src/cmds

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re create_obj_dirs
