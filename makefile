chatclient:
	gcc -o chatclient chatclient.c

default: chatclient.o

run: chatclient.o
	./chatclient ${HOSTNAME} ${PORT}

clean:
	rm -f chatclient