CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = lexical_analyser
SRC = main.c lexer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)