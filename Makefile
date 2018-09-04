CC       ?= gcc
CPPFLAGS ?=
CFLAGS   ?= -std=gnu99 -ggdb3 -O0 -Wall -Werror
LDFLAGS  ?=

all: librbt.so main

librbt.so: rbt.h rbt.c rbt_insert.c rbt_remove.c rbt_util.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -fPIC -o rbt.o rbt.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -fPIC -o rbt_insert.o rbt_insert.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -fPIC -o rbt_remove.o rbt_remove.c
	$(CC) -shared -o librbt.so rbt.o rbt_insert.o rbt_remove.o

main: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o main main.c -L$(PWD) -lrbt

clean:
	$(RM) librbt.so rbt.o rbt_insert.o rbt_remove.o main

.PHONY: all clean
