
.PHONY: all build run clean test

include sources.mk

PLATFORM=HOST
TARGET=project1

CC=gcc
CSTD=c99

INCLUDE=-I./inc


CFLAGS=-std=$(CSTD)\
	-lpthread\
	-Wall\
	-lm\
	-g
#	-O0\
#	-Werror

MOCKED_FUNCTIONS=read_reg_temp\
	read_reg_light_word

WRAP_FLAG=-Wl,--wrap=
WRAPPED_FUNCTIONS=$(addprefix $(WRAP_FLAG),$(MOCKED_FUNCTIONS))

TESTFLAGS=-lcmocka

#objest files for different scenarios
MAIN_OBJECT=$(MAIN_SOURCE:.c=.o)
COMMON_OBJECTS=$(COMMON_SOURCES:.c=.o)
BBB_OBJECTS=$(BBB_SOURCES:.c=.o)
HOST_OBJECTS=$(HOST_SOURCES:.c=.o)
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)

TEST_TARGETS=$(TEST_SOURCES:.c=.test)

# Change compiler and files if cross compiling for BBB/BBG\
also removes the I2C sensor files\
when compiling for HOST
ifeq ($(PLATFORM),BBB)
CC=arm-linux-gnueabihf-gcc
OBJECTS= $(COMMON_OBJECTS) $(BBB_OBJECTS) $(MAIN_OBJECT)
CFLAGS+=-DBBB
else
OBJECTS=$(COMMON_OBJECTS) $(HOST_OBJECTS) $(MAIN_OBJECT)
endif

#default build , changes based on platform
build: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS) $(INCLUDE)

run: build
ifeq ($(PLATFORM),BBB)
	scp $(TARGET) root@192.168.7.2:/home/proj1/$(TARGET)
else
	./$(TARGET) -f proj1logfile.log
endif

all: run

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

client:$(CLIENT_SOURCES)
	$(CC) -o $@ $(CLIENT_SOURCES) $(INCLUDE)
ifeq ($(PLATFORM),BBB)
	scp $@ root@192.168.7.2:/home/proj1/$@
endif


#rule to run unit tests
test: $(TEST_TARGETS)
	./$^

# generates executable for each unit test file\
and runs them
%.test:%.c $(BBB_OBJECTS) $(COMMON_OBJECTS)
	$(CC) $< $(COMMON_OBJECTS) $(BBB_OBJECTS) -o $@ $(CFLAGS) $(INCLUDE) \
	$(TESTFLAGS) $(WRAPPED_FUNCTIONS)

clean:
	rm -f src/*.o test/*.o
	rm -f $(TEST_TARGETS) $(TARGET) client

