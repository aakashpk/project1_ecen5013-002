.PHONY: all clean unittest

include sources.mk

PLATFORM=HOST

TARGET=project1

CC=gcc
CSTD=c99

ifeq ($(PLATFORM),BBB)
CC=arm-linux-gnueabihf-gcc
endif

INCLUDE=-I./inc
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
ifeq ($(PLATFORM),BBB)
	$(CC) $(objects) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	scp $(TARGET) root@192.168.7.2:/home/proj1/$(TARGET)
else
	$(CC) $(objects) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	./$(TARGET)
endif

#bbb:$(objects)
#	$(CC) $(objects) -o $(TARGET) $(CFLAGS) $(INCLUDE)
#	scp $(TARGET) root@192.168.7.2:/home/proj1/$(TARGET)


unittest:$(TESTOBJECTS)
	$(CC) $(TESTOBJECTS) -o $(TESTTARGET) $(CFLAGS) $(INCLUDE) $(TESTFLAG)
	./$(TESTTARGET)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)


clean:
	rm -f src/*.o test/*.o 
	rm -f $(TESTTARGET) $(TARGET)

