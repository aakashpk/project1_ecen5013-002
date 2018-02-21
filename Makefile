.PHONY: all clean unittest

include sources.mk

TARGET=project1

#CC=arm-linux-gnueabihf-gcc
CC=gcc
CSTD=c99

INCLUDE=-I../inc
#SOURCEDIR=src/
#TESTDIR=test/
#BINDIR=bin/

TESTTARGET=project1Test

TESTFLAG=-lcmocka

CFLAGS=-std=$(CSTD)\
	-Wall
#	-O0\
#	-Werror

objects=$(SOURCES:.c=.o)

TESTOBJECTS=$(TESTSOURCES:.c=.o)


all:$(objects)
	$(CC) $(objects) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	./$(TARGET)

unittest:$(TESTOBJECTS)
	$(CC) $(TESTOBJECTS) -o $(TESTTARGET) $(CFLAGS) $(INCLUDE) $(TESTFLAG)
	./$(TESTTARGET)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)


clean:
	rm -f src/*.o test/*.o 
	rm -f $(TESTTARGET) $(TARGET)

