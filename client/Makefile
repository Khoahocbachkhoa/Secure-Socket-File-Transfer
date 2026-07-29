CC = gcc

CFLAGS = -I./include

TARGET = client.out

SRC = $(shell find src -name "*.c")

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) -lcrypto -lssl
	rm -f $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

run: all
	./$(TARGET)