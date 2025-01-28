CC = gcc
CCPP = g++

CFLAGS = -Wall -Wextra -O3
AR = ar

LIB_NAME = libmtc_decoder.a
TEST_EXEC = mtc_tests

SRCS = src/mtccdecoder.c src/tmtcdecoder.c
TEST_SRC = tests/mtc-tests.cpp

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRC:.cpp=.o)

INCLUDES = -Iinclude -I../config

all: $(LIB_NAME)
all: CFLAGS += -std=c17

test: $(TEST_EXEC)
test: CFLAGS += -std=c++17


# Build the static library
$(LIB_NAME): $(OBJS)
	$(AR) rcs $@ $^

# Build the test executable
$(TEST_EXEC): $(TEST_OBJS)
	$(CCPP) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIB_NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(LIB_NAME) $(TEST_OBJS) $(TEST_EXEC)

.PHONY: all clean
