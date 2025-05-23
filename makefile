shell:
	gcc -g -I include src/*.c -export-dynamic -o cshell

cmds:
	make -C src/commands/ build

cmds_clean:
	make -C src/commands/ clean 

run:
	make shell
	./cshell

all:
	make shell
	make cmds

valgrind:
	make all
	valgrind ./cshell
