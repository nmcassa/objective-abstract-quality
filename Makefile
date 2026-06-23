CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -fPIC
LDFLAGS := -shared

SRC     := src/parser.c src/analysis.c src/submission.c src/reviewer.c src/oaq_api.c
OBJ     := $(SRC:.c=.o)
BIN     := oaq
LIB     := liboaq.so          # use .dylib on macOS, oaq.dll on Windows

all: $(BIN) $(LIB)

$(BIN): src/main.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(LIB): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BIN) $(LIB)

.PHONY: all clean
