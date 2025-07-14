SRC_DIR := src
UNI_DIR := hot

OBJ_DIR := build
SHR_DIR := units

SRCS := $(wildcard $(SRC_DIR)/*.c)
UNTS := $(wildcard $(UNI_DIR)/*.c)

OUT_RUST_LIB := $(OBJ_DIR)/libcshell.so 
SRC_RUST_LIB := target/release/libcshell.so

OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o,$(SRCS)) $(OUT_RUST_LIB)
SHRD := $(patsubst $(UNI_DIR)/%.c, $(SHR_DIR)/%.so,$(UNTS))

OUT := luall
BUNDLE := $(SHR_DIR)/bundle.so

C := gcc
CFLAGS := -g -shared -I include -c -Wall -Werror
UFLAGS := -g -shared -I include -fPIC -Wall -Werror

LDFLAGS := -o $(OUT) -export-dynamic -llua

all: shell hot

shell: $(OUT)

$(OUT): $(OBJS)
	$(C) $(OBJS) $(LDFLAGS)

$(OUT_RUST_LIB): Cargo.toml
	cargo build --release
	cp $(SRC_RUST_LIB) $(OUT_RUST_LIB)
	cbindgen -c ./cbindgen.toml --output include/bindgen.h

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

clean_all: clean clean_units

valgrind: shell
	valgrind ./$(OUT)


.PHONY: all clean cmds source $(OUT_RUST_LIB)

