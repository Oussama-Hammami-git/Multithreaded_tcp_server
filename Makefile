CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
TARGET = multithreaded_tcp
SRCS = multithreaded_tcp.c my_queue.c
OBJS = $(SRCS:.c=.o)
HEADERS = my_queue.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
