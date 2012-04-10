ep1: main.c libs/LL.c
	gcc main.c libs/LL.c -lpthread -g -o ep1

clean:
	rm ep1
