CC = gcc
CFLAGS = -std=c++17 -fPIC -ldl

LIB_NAME = mem_tracker.so
LIB_TEST = mem_tracker_test

SRCS = memory-tracker.cpp
TEST_SRCS = tracker-test.cpp

OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all: $(LIB_NAME)

test: $(LIB_TEST)


$(LIB_NAME): $(OBJS)
	$(CC) -shared -o $@ $^

$(LIB_TEST): $(TEST_OBJS)
	$(CC) -o $(LIB_TEST) $(TEST_OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(LIB_NAME) $(TEST_OBJS) $(LIB_TEST)

.PHONY: all clean
