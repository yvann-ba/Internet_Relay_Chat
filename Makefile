NAME = ./ircserv
NAME_BONUS = ./ircserv_bonus
CPP = /bin/c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp src/server.cpp src/client.cpp src/utils.cpp src/channel.cpp \
		src/cmds/passCommand.cpp src/cmds/nickCommand.cpp src/cmds/userCommand.cpp \
		src/cmds/joinCommand.cpp src/cmds/kickCommand.cpp src/cmds/inviteCommand.cpp \
		src/cmds/modeCommand.cpp src/cmds/privmsgCommand.cpp src/cmds/topicCommand.cpp \
		src/cmds/quitCommand.cpp

SRCS_BONUS = main.cpp bonus/server_bonus.cpp src/client.cpp src/utils.cpp src/channel.cpp \
		src/cmds/passCommand.cpp src/cmds/nickCommand.cpp src/cmds/userCommand.cpp \
		src/cmds/joinCommand.cpp src/cmds/kickCommand.cpp src/cmds/inviteCommand.cpp \
		src/cmds/modeCommand.cpp src/cmds/privmsgCommand.cpp src/cmds/topicCommand.cpp \
		src/cmds/quitCommand.cpp \
		bonus/bot.cpp

OBJ_DIR = ./obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
OBJS_BONUS = $(SRCS_BONUS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

bonus: $(NAME_BONUS)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(NAME_BONUS): $(OBJS_BONUS)
	$(CPP) $(CPPFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) -c $< -o $@ $(CPPFLAGS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS)

re: fclean all

.PHONY: all clean fclean re
