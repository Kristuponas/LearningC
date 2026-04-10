CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lcurl

TARGET = speedtestd
SRC = speedtestd.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
