TARGET = webserver
EXENAME = webserver

CC = gcc
CFLAGS = -Wall -Wextra
DEPS = http.o server.o init.o
OBJS = $(TARGET).o

all: $(TARGET)

install: $(TARGET) clean

$(TARGET): $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $(EXENAME) $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o
distclean: clean
	rm -f $(EXENAME)

.PHONY: clean

