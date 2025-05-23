build_shell:
	gcc -g -I include src/*.c -o cshell

build_cmds:
	gcc -g -I include src/commands/cd.c -shared -o utils/cd.so

run:
	make build_shell
	./cshell

build_all:
	make build_shell
	make build_cmds

valgrind:
	make build_all
	valgrind ./cshell
