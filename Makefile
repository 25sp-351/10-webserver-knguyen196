CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

SRC = \
    main.c \
    echo.c \
    calc/calc.c \
    http/request.c \
    http/response.c \
    static/static.c

OBJ = $(SRC:.c=.o)
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)