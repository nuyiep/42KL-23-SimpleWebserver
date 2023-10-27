NAME		= sean
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
# CXXFLAGS	+= -fsanitize=address -g3
SRCS		= $(wildcard *.cpp)


$(NAME):
	@clear
	@$(CXX) $(CXXFLAGS) ${SRCS} -o $(NAME)

fclean:
	@rm -rf ${NAME}

re:
	@make fclean
	@make