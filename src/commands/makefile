files := $(wildcard *.c)

build:
	$(foreach file, $(files), gcc -I ../../include $(file) -shared -o ../../utils/$(file).so &&) echo "build cmds!"

clean:
	rm ../../utils/*.so
