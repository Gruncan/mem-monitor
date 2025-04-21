CC = gcc
CCPP = g++

CFLAGS = -Wall -Wextra -O3
AR = ar

LIB_NAME = libmtc_decoder.a
LIB_NAME_SHR = libmtc_decoder.so
TEST_EXEC = mtc_tests
TEST_TMTC_EXEC = tmtc_tests
TEST_STREAM_EXEC = tmtc_stream_tests

SRCS = src/mtccdecoder.c src/tmtcdecoder.c
TEST_SRC = tests/mtc-tests.cpp
TEST_TMTC_SRC = tests/tmtc-tests.cpp
TEST_STREAM_SRC = tests/tmtc-stream-tests.cpp

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRC:.cpp=.o)
TEST_TMTC_OBJS = $(TEST_TMTC_SRC:.cpp=.o)
TEST_STREAM_OBJS = $(TEST_STREAM_SRC:.cpp=.o)

INCLUDES = -Iinclude -I../config

all: $(LIB_NAME)
all: CFLAGS += -std=c17

shared: $(LIB_NAME_SHR)
shared: CFLAGS += -fPIC

test: $(TEST_EXEC)
test: CFLAGS += -std=c++17 -DMEM_TEST

test_tmtc: $(TEST_TMTC_EXEC))
test_tmtc: CFLAGS += -std=c++17 -DMEM_TEST

teststream: $(TEST_STREAM_EXEC)
teststream: CFLAGS += -std=c++17 -DMEM_TEST


$(LIB_NAME): $(OBJS)
	$(AR) rcs $@ $^

$(LIB_NAME_SHR): $(OBJS)
	$(CC) -shared -o $@ $^

$(TEST_EXEC): $(TEST_OBJS)
	$(CCPP) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIB_NAME)

$(TEST_TMTC_EXEC): $(TEST_TMTC_OBJS)
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
