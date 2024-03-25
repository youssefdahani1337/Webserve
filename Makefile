CC= c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = WebServ

SRC = cgi.cpp handleResponse.cpp parseLocation.cpp Response.cpp Client.cpp Location.cpp ParseRequest.cpp sendResponse.cpp \
Configuration.cpp main.cpp parseServer.cpp Server.cpp Delete.cpp Manager.cpp Post.cpp statusCode.cpp \
fillEnv.cpp Message.cpp Request.cpp Tools.cpp

SRCS = $(addprefix ./srcs/, $(SRC))

OBJECTDIR = ./builds/

OBJECT = $(addprefix $(OBJECTDIR), $(notdir $(SRCS:.cpp=.o)))

H = Manager.hpp Server.hpp Client.hpp Message.hpp Request.hpp Response.hpp Location.hpp Tools.hpp Macros.hpp\

HEADERS = $(addprefix ./include/, $(H))

all : $(NAME) $(OBJECTDIR)

$(OBJECTDIR) :
	mkdir -p $(OBJECTDIR)
	
$(OBJECTDIR)%.o : ./srcs/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJECT) 
	@$(CC) $(CPPFLAGS) $(OBJECT) -o $(NAME)
	@echo "\033[0;35mcompiled !!!!\033[0m"

clean : 
	@rm -rf $(OBJECT)
	@echo "\033[1;36mall clean\033[0m"
	
fclean : clean
	@rm -rf $(NAME)

re : fclean all

.SILENT : $(OBJECT)