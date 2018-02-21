.PHONY: all clean unittest

include sources.mk

TARGET=project1

CC=gcc
CSTD=c99

INCLUDE=-I../inc

TESTTARGET=projectTest

TESTFLAG=-lcmocka

CFLAGS=-std=$(CSTD)\
	-Wall
#	-O0\
#	-Werror

objects=$(sources:.c=.o)

all:$(objects)
	$(CC) $(objects) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	./$(TARGET)

unittest:$(TESTOBJECTS)
	$(CC) $(TESTOBJECTS) -o $(TESTTARGET) $(CFLAGS) $(INCLUDE) $(TESTFLAG)


%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)


clean:
	rm -f *.o *.elf
