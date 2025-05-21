CC      := gcc
CFLAGS  := -Wall -Wextra -g
TARGET  := desafio2

.PHONY: all clean

all: $(TARGET)

$(TARGET): desafio2.c funciones.c funciones.h
	$(CC) $(CFLAGS) desafio2.c funciones.c -o $(TARGET)

clean:
	rm -f $(TARGET)