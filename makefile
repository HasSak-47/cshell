SRC_DIR := src
OBJ_DIR := .cache
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
OUT := cshell

C := gcc
CFLAGS := -g -I include -export-dynamic

LDFLAGS := -o $(OUT)

shell: source
	$(C) $(CFLAGS) $(OBJS) $(LDFLAGS)

run : shell
	./$(OUT)

all: source inst
	$(C) $(CFLAGS) $(OBJS) $(LDFLAGS)

source: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(C) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

cmds: src/commands
	$(MAKE) -C src/commands

clean:
	rm $(OBJS)

valgrind: all
	valgrind ./$(OUT)

.PHONY: all clean cmds source

# SRC_DIR = src
# SRCS= $(WILDCARD $(SRC_DIR)/*.c)
# OBJS = $(patsub $SRCS:.c=.o)
# 
# INCLUDE = include
# 
# C = gcc
# CFLAGS = -g -I $(INCLUDE) --export-dynamic
# 
# 
# shell: 
# 	gcc -g -I include src/*.c -export-dynamic -o cshell
# 
# run: shell
# 	./cshell
# 
# all: shell cmds
# 	make shell
# 	make cmds
# 
# 
# cmds:
# 	make -C src/commands/ build
# 
# cmds_clean:
# 	make -C src/commands/ clean 
# 
# valgrind: all
# 	valgrind ./cshell
