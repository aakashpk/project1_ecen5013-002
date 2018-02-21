.PHONY: all clean unittest

include sources.mk

TARGET=project1

#CC=arm-linux-gnueabihf-gcc
CC=gcc
CSTD=c99

INCLUDE=-I../inc

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

unittest:$(TESTSOURCES)
	echo $(TESTOBJECTS) $(TESTSOURCES)
	$(CC) $(TESTSOURCES) -o $(TESTTARGET) $(CFLAGS) $(INCLUDE) $(TESTFLAG)
	./$(TESTTARGET)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

../test/%.o: test/%.c
	echo $(TESTOBJECTS) $(TESTSOURCES)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)



clean:
	rm -f *.o *.elf
