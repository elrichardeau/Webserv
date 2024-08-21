CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98

SOURCES = main.cpp \
		  Config.cpp \
		  LocationConfig.cpp \
		  ServerConfig.cpp \
		  Requests.cpp \
		  Server.cpp \
		  ServerManager.cpp \
		  ErrorPage.cpp \
		  Cgi.cpp \
		  utils.cpp

SRC = srcs/
DIR = objs_deps/

SRCS = $(addprefix $(SRC), $(SOURCES))
OBJS = $(addprefix $(DIR), $(OBJ))
DEPS = $(addprefix $(DIR), $(DEP))

OBJ = $(SOURCES:.cpp=.o)
DEP = $(SOURCES:.cpp=.d)

NAME = webserv

all : $(NAME)

$(NAME) : $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "Compilation completed"

$(DIR)%.o : $(SRC)%.cpp | $(DIR)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

$(DIR) :
	@echo "Start Compilation for $(NAME)"
	@echo "Wait ..."
	@mkdir -p $(DIR)

clean :
	@echo "Deleting Objects and Dependencies"
	@rm -rf $(DIR)

fclean : clean
	@echo "Deleting the executable"
	@rm -f $(NAME)

re : fclean all

-include $(DEPS)

.PHONY : all bonus clean fclean re