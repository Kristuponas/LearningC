CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = watchdogd
SRC = watchdogd.c

DUMMY_TARGET = dummy
DUMMY_SRC = dummy.c

all: $(TARGET) $(DUMMY_TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

$(DUMMY_TARGET): $(DUMMY_SRC)
	$(CC) $(CFLAGS) $(DUMMY_SRC) -o $(DUMMY_TARGET)

clean:
	rm -f $(TARGET)
