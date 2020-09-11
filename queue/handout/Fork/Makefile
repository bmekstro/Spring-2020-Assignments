########################################################
# Variable definitions
########################################################
# C compiler
CC      = gcc

# C compiler flags
CFLAGS = -g -Wall -Wextra -Werror -pedantic
#CFLAGS = -O3 -Wall -Wextra -Werror -pedantic

# Linker flags
LDFLAGS  := 

# Libraries we're using, prefaced with "-l"
LDLIBS   := 

#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

Fork : Fork.o
	$(LINK.o) $(LDFLAGS) $^ $(LDLIBS) -o $@

Fork.o : Fork.c

#############################################################

.PHONY : clean

clean :
	@$(RM) *.o *~ *.d

#############################################################