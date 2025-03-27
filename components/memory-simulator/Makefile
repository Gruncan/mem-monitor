CC = g++
CFLAGS = -std=c++17 -I./include -I../common/mtc/include -I../common/config
LIBS = ../common/mtc/libmtc_decoder.a


TARGET = memorysimulator
TEST_TARGET = memorysimulator_test

SRCS = src/main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_OBJS) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno coverage.info coverage-report/

.PHONY: all clean
