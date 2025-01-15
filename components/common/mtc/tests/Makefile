CC = g++

CFLAGS = -I../include -L. -l:./libmt

TARGET = mtcctest

SRCS = mtc-tests.cpp ../src/mtccdecoder.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)

.PHONY: all clean
