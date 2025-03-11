CC = gcc
CCPP = g++

CFLAGS = -Wall -Wextra -O3
AR = ar

LIB_NAME = libmtc_decoder.so
TEST_EXEC = mtc_tests
TEST_STREAM_EXEC = tmtc_stream_exec

SRCS = src/mtccdecoder.c src/tmtcdecoder.c
TEST_SRC = tests/tmtc-tests.cpp
TEST_STREAM_SRC = tests/tmtc-stream-tests.cpp

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRC:.cpp=.o)
TEST_STREAM_OBJS = $(TEST_STREAM_SRC:.cpp=.o)

INCLUDES = -Iinclude -I../config

all: $(LIB_NAME)
all: CFLAGS += -std=c17

test: $(TEST_EXEC)
test: CFLAGS += -std=c++17

teststream: $(TEST_STREAM_EXEC)
teststream: CFLAGS += -std=c++17

$(LIB_NAME): $(OBJS)
	$(AR) rcs $@ $^

$(TEST_EXEC): $(TEST_OBJS)
	$(CCPP) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIB_NAME)

$(TEST_STREAM_EXEC): $(TEST_STREAM_OBJS)
	$(CCPP) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIB_NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(LIB_NAME) $(TEST_OBJS) $(TEST_EXEC) $(TEST_STREAM_OBJS) $(TEST_STREAM_EXEC)

.PHONY: all clean
