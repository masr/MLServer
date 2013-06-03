install:main.o resource_action.o php_process.o
	gcc main.o resource_action.o php_process.o -o mlserver
main.o:main.c
	gcc -c main.c -o main.o
resource_action.o:resource_action.c
	gcc -c resource_action.c -o resource_action.o
php_process.o:php_process.c
	gcc -c php_process.c -o php_process.o
clean:
	rm mlserver *.o
