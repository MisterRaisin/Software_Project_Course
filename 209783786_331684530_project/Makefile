CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
TARGET = symnmf
SRC = symnmf.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm

clean:
	rm -f $(TARGET)

.PHONY: all clean