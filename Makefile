.PHONY: all clean unittest

include sources.mk

PLATFORM=HOST
TARGET=project1

CC=gcc
#CSTD=c99

CFLAGS=-Wall

#CFLAGS=-std=$(CSTD)\
	-Wall
#	-O0\
#	-Werror

ifeq ($(PLATFORM),BBB)
CC=arm-linux-gnueabihf-gcc
OBJECTS=$(COMMON_SOURCES:.c=.o) $(BBB_SOURCES:.c=.o)
CFLAGS+=-DBBB
else
OBJECTS=$(COMMON_SOURCES:.c=.o) $(HOST_SOURCES:.c=.o)
endif

INCLUDE=-I./inc

TESTTARGET=project1Test

TESTFLAG=-lcmocka

TESTOBJECTS=$(TESTSOURCES:.c=.o)


all:$(OBJECTS)
ifeq ($(PLATFORM),BBB)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	scp $(TARGET) root@192.168.7.2:/home/proj1/$(TARGET)
else
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS) $(INCLUDE)
	./$(TARGET)
endif


unittest:$(TESTOBJECTS)
	$(CC) $(TESTOBJECTS) -o $(TESTTARGET) $(CFLAGS) $(INCLUDE) $(TESTFLAG)
	./$(TESTTARGET)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)


clean:
	rm -f src/*.o test/*.o 
	rm -f $(TESTTARGET) $(TARGET)

