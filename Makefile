CC = gcc

CFLAGS = -std=c11 -I./source -pthread

TARGET = mem_monitor
TEST_TARGET = mem_monitor_test

SRCS = main.c source/mem-info.c source/mem-writer.c source/mem-threading.c source/mem-reader.c source/process-reader.c
TEST_SRCS = tests/test.c source/mem-info.c source/mem-writer.c source/mem-threading.c source/mem-reader.c source/process-reader.c

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


test: $(TEST_TARGET)
test: CFLAGS += -I./tests -g -fprofile-arcs -ftest-coverage -DMEM_TEST -O0 -fsanitize=address


$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno source/*.gcda source/*.gcno coverage.info coverage-report/

.PHONY: all clean
