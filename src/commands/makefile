UTILS := ../../utils
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, $(UTILS)/%.so,$(SRCS))

C := gcc
CFLAGS := -g -I ../../include -shared

build: $(OBJS)

$(UTILS)/%.so: %.c | $(UTILS)
	$(C) $(CFLAGS) -c $< -o $@

$(UTILS):
	mkdir -p $(UTILS)

clean:
	rm ../../utils/*.so

.PHONY: build clean
