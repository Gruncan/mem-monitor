CC = gcc

CFLAGS = -std=c11 -I./include -I../common/config -pthread -O3 -s -funroll-loops -flto -ffast-math


VERSION_1 :=
ifdef VERSION_1
	CFLAGS += -DVERSION_1
endif

TARGET = mem_monitor
TARGET_DEBUG = mem_monitor_debug
TEST_TARGET = mem_monitor_test

SRCS = src/main.c src/mem-info.c src/mem-writer.c src/mem-threading.c src/mem-reader.c src/process-reader.c
TEST_SRCS = tests/test.c src/mem-info.c src/mem-writer.c src/mem-threading.c src/mem-reader.c src/process-reader.c

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

debug: $(TARGET_DEBUG)
debug: CFLAGS = -std=c11 -I./include -I../common/config -pthread -g -DMEM_DEBUG


test: $(TEST_TARGET)
test: CFLAGS += -I../common/testing -g -fprofile-arcs -ftest-coverage -DMEM_TEST


$(TARGET_DEBUG): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_DEBUG) $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno coverage.info coverage-report/ *.mtc

.PHONY: all clean
