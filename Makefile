SillyShell: silly.c
	gcc -g -Wall -o SillyShell silly.c
test:
	make && valgrind ./SillyShell
run:
	./SillyShell
clean:
	rm SillyShell
