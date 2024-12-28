CC = g++
CFLAGS = -std=c++17 -fPIC -O3 -march=native -funroll-loops -flto -ffast-math

LIB_NAME = lib_mem_tracker.so
LIB_TEST = mem_tracker_test

SRCS = src/memory-tracker.cpp
TEST_SRCS = tests/tracker-test.cpp

OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all: $(LIB_NAME)

test: $(LIB_TEST)
test: CFLAGS += -I../common


$(LIB_NAME): $(OBJS)
	$(CC) -shared -o $@ $^

$(LIB_TEST): $(TEST_OBJS)
	$(CC) -o $(LIB_TEST) $(TEST_OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(LIB_NAME) $(TEST_OBJS) $(LIB_TEST)

.PHONY: all clean
