CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2
SRC     := src/main.c src/parser.c
OBJ     := $(SRC:.c=.o)
BIN     := oaq

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
