CC = gcc

CFLAGS = -std=c11 -I./include -I../common -pthread -O3 -march=native -funroll-loops -flto -ffast-math

TARGET = mem_monitor
TEST_TARGET = mem_monitor_test

SRCS = src/main.c src/mem-info.c src/mem-writer.c src/mem-threading.c src/mem-reader.c src/process-reader.c
TEST_SRCS = tests/test.c src/mem-info.c src/mem-writer.c src/mem-threading.c src/mem-reader.c src/process-reader.c

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


test: $(TEST_TARGET)
test: CFLAGS += -I../common -g -fprofile-arcs -ftest-coverage -DMEM_TEST


$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno coverage.info coverage-report/ *.mtc

.PHONY: all clean
