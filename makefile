SRC_DIR := src
UNI_DIR := hot

OBJ_DIR := build
SHR_DIR := units

SRCS := $(wildcard $(SRC_DIR)/*.c)
UNTS := $(wildcard $(UNI_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o,$(SRCS))
SHRD := $(patsubst $(UNI_DIR)/%.c, $(SHR_DIR)/%.so,$(UNTS))

OUT := luall
BUNDLE := $(SHR_DIR)/bundle.so

C := gcc
CFLAGS := -g -shared -I include -c
UFLAGS := -g -shared -I include -fPIC

LDFLAGS := -o $(OUT) -export-dynamic -llua

all: $(OUT) hot

$(OUT): $(OBJS)
	$(C) $(OBJS) $(LDFLAGS)

shell: $(OUT)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(C) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run : shell
	./$(OUT)

hot: $(BUNDLE)

$(BUNDLE): $(SHRD)
	$(C) -fPIC -shared $(SHRD) -o $(BUNDLE)


$(SHR_DIR)/%.so: $(UNI_DIR)/%.c | $(SHR_DIR)
	$(C) $(UFLAGS) $< -o $@

$(SHR_DIR):
	mkdir -p $(SHR_DIR)

clean:
	rm $(OBJS)

clean_units:
	rm $(SHR_DIR)/bundle.so
	rm $(SHRD)

valgrind: shell
	valgrind ./$(OUT)


.PHONY: all clean cmds source

