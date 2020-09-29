CC = gcc
CFLAGS = -Wall -Wextra
DEPS = webserver.o http.o server.o init.o

webserver: webserver.o http.o server.o init.o
	$(CC) $(CFLAGS) -o $@ $^

webserver.o: webserver.c
	$(CC) $(CLAGS) -o $@ -c $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
distclean: clean
	rm -f webserver *.out
