USERNAME		= user
USEROBJ_PATH 	= userObj

USERSRCS		= $(shell find userSrc/ -name '*.cpp')

USEROBJS 		= $(addprefix $(USEROBJ_PATH)/, $(addsuffix .o, $(basename $(USERSRCS))))
USERINCLUDES	= userInc/

ASNAME		= AS
ASOBJ_PATH 	= asObj

ASSRCS		= $(shell find asSrc/ -name '*.cpp')

ASOBJS 		= $(addprefix $(ASOBJ_PATH)/, $(addsuffix .o, $(basename $(ASSRCS))))
ASINCLUDES	= asInc/

CXX			= c++
USERCXXFLAGS	= -Wall -Wextra -Werror -I$(USERINCLUDES) -g
ASCXXFLAGS	= -Wall -Wextra -Werror -I$(ASINCLUDES) -g
RM			= rm -f

all: user as

user: $(USERNAME)

$(USEROBJ_PATH)/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(USERCXXFLAGS) -I $(USERINCLUDES) -I /usr/local/include -c -o $@ $<

$(USERNAME): $(USEROBJS)
	$(CXX) $(USERCXXFLAGS) -o $@ $^

as: $(ASNAME)

$(ASOBJ_PATH)/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(ASCXXFLAGS) -I $(ASINCLUDES) -I /usr/local/include -c -o $@ $<

$(ASNAME): $(ASOBJS)
	$(CXX) $(ASCXXFLAGS) -o $@ $^

clean: cleanuser cleanas

cleanuser:
	rm -rf $(USEROBJ_PATH)
	rm -rf $(USERNAME)
cleanas:
	rm -rf $(ASOBJ_PATH)
	rm -rf $(ASNAME)
