MAIN_SOURCE=src/main.c

COMMON_SOURCES=src/socketserver.c \
			src/logger.c \
			src/common_queue.c \
			src/bidirectional_queue.c \
			src/single_ended_queue.c\
			src/tasks.c

BBB_SOURCES=src/tempsensor.c\
	src/my_i2c.c\
	src/lightsensor.c\
	src/sensordata.c

HOST_SOURCES=src/dummydata.c

WRAPPED_TEST_SOURCES:=test/sensor_test.c

TEST_SOURCES=test/simple_test.c \
		   test/queue_test.c

CLIENT_SOURCES=src/socketclient.c

MOCKED_FUNCTIONS=
#	test/bd_queue_test.c


