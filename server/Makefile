# Makefile for the TCP Echo Server

CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
TARGET = server
SRC = server.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lm

clean:
	rm -f $(TARGET)

