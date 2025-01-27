CC = g++

CFLAGS = -std=c11 -I./include -I../common/mtc/include -pthread -O3 -march=native -funroll-loops -flto -ffast-math

TARGET = memorysimulator
TEST_TARGET = memorysimulator_test

SRCS = src/main.cpp
TEST_SRCS = src/main.cpp

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
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno coverage.info coverage-report/ *.mtc *tmtc

.PHONY: all clean
