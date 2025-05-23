build_shell:
	gcc -I include src/*.c -o cshell

build_cmds:
	gcc -I include src/commands/cd.c -shared -o utils/cd.sjjjo

run:
	make build_shell
	./cshell
