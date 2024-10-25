CC = gcc
CFLAGS = -std=c11 -I./source -pthread

TARGET = mem_monitor

SRCS = main.c source/mem-info.c source/mem-writer.c source/mem-threading.c source/mem-reader.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
