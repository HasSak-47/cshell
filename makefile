SRC_DIR := src
UNI_DIR := src/commands

OBJ_DIR := build
SHR_DIR := units

SRCS := $(wildcard $(SRC_DIR)/*.c)
UNTS := $(wildcard $(UNI_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o,$(SRCS))
SHRD := $(patsubst $(UNI_DIR)/%.c, $(SHR_DIR)/%.so,$(UNTS))

OUT := luall

C := gcc
CFLAGS := -g -I include -c
UFLAGS := -g -shared -I include

LDFLAGS := -o $(OUT) -export-dynamic -llua

all: $(OUT) units

$(OUT): $(OBJS)
	$(C) $(OBJS) $(LDFLAGS)

shell: $(OUT)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(C) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run : shell
	./$(OUT)

cmd : $(SHRD)

$(SHR_DIR)/%.so: $(UNI_DIR)/%.c | $(SHR_DIR)
	$(C) $(UFLAGS) $< -o $@

$(SHR_DIR):
	mkdir -p $(SHR_DIR)

clean:
	rm $(OBJS)

clean_units:
	rm $(SHRD)

valgrind: shell
	valgrind ./$(OUT)


GCC_PLUGINS_DIR = $(shell gcc -print-file-name=plugin)
CXXFLAGS+= -I$(GCC_PLUGINS_DIR)/include -fPIC -fno-rtti

plugins: extensions/vectored.c
	g++ -shared $(CXXFLAGS) extensions/vectored.c -o plugins/bin


.PHONY: all clean cmds source

