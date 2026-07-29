CC = gcc

CFLAGS = -O2 -g -I./include

SRC = $(shell find src -name "*.c")

TARGET = server.out

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lpq -lcrypto -lssl

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all run clean re