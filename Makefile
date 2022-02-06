SillyShell: silly.c
	gcc -g -Wall -o SillyShell silly.c
test:
	make && valgrind --trace-children=yes --child-silent-after-fork=yes ./SillyShell
run:
	./SillyShell
clean:
	rm SillyShell
