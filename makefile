SRC_DIR := src
UNI_DIR := src/commands

OBJ_DIR := .cache
SHR_DIR := units

SRCS := $(wildcard $(SRC_DIR)/*.c)
UNTS := $(wildcard $(UNI_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o,$(SRCS))
SHRD := $(patsubst $(UNI_DIR)/%.c, $(SHR_DIR)/%.so,$(UNTS))

OUT := luall

C := gcc
CFLAGS := -g -I include
UFLAGS := -g -I include -shared -fpic

LDFLAGS := -o $(OUT) -export-dynamic

all: $(OUT) units

$(OUT): $(OBJS)
	$(C) $(CFLAGS) $(OBJS) $(LDFLAGS)

shell: $(OUT)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(C) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run : shell
	./$(OUT)

cmd : $(SHRD)

$(SHR_DIR)/%.so: $(UNI_DIR)/%.c | $(SHR_DIR)
	$(C) $(UFLAGS) -c $< -o $@

$(SHR_DIR):
	mkdir -p $(SHR_DIR)

clean:
	rm $(OBJS)

clean_units:
	rm $(SHRD)

valgrind: shell
	valgrind ./$(OUT)

.PHONY: all clean cmds source
