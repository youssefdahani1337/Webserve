define convert_to_objs
$(addprefix $(1)/, $(patsubst %.c,$(2)%.o,$(notdir $(3))))
endef


CC= c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

NAME = WebServ

SRCS = main.cpp Configuration.cpp  Manager.cpp Server.cpp parseServer.cpp parseLocation.cpp Location.cpp Client.cpp Message.cpp Request.cpp Response.cpp Post.cpp ParseRequest.cpp handleResponse.cpp Tools.cpp Delete.cpp cgi.cpp statusCode.cpp fillEnv.cpp

OBJDIR = builds

OBJECT =$(call convert_to_objs , srcs, $(OBJDIR), $(SRCS)) \

H = Manager.hpp Server.hpp Client.hpp Message.hpp Request.hpp Response.hpp Location.hpp Tools.hpp Macros.hpp
HEADERS_DIR = ./include/

HEADERS = $(H:%.hpp=$(HEADERS_DIR)%.hpp)

all : $(NAME) 

$(OBJECT) : | $(OBJDIR)

$(OBJDIR) :
	@mkdir $(OBJDIR)

$(OBJDIR)/%.o : %.cpp 
	@$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJECT) $(HEADERS)
	@$(CC) $(CPPFLAGS) $(OBJECT) -o $(NAME)
	@echo "\033[0;35mcompiled !!!!\033[0m"

clean : 
	@rm -rf $(OBJDIR)
	@echo "\033[1;36mall clean\033[0m"
	
fclean : clean
	@rm -rf $(NAME)

re : fclean all

.SILENT : $(OBJECT)