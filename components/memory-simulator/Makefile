CC = g++
CFLAGS = -std=c++17 -I./include -I../common/mtc/include -I../common/config
LDFLAGS = -L../common/mtc/
LIBS = -lmtc_decoder


TARGET = memorysimulator
TEST_TARGET = memorysimulator_test

SRCS = src/main.cpp
#TEST_SRCS = src/main.cpp

OBJS = $(SRCS:.cpp=.o)
#TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

#test: $(TEST_TARGET)
#test: CFLAGS += -I../common -g -fprofile-arcsmake -ftest-coverage -DMEM_TEST
#
#
#$(TEST_TARGET): $(TEST_OBJS)
#	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno coverage.info coverage-report/

.PHONY: all clean
