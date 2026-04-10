CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = healthcheckd
SRC = healthcheckd.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
