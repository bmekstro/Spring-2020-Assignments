CC := gcc
CFLAGS := -O1 -Wall -Wextra -Werror -std=c11 -g

.PHONY : all clean

all : create_fs runner

runner.c : fs.c

clean :
	$(RM) create_fs runner
