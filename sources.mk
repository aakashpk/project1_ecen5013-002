MAIN_SOURCE=src/main.c

COMMON_SOURCES=src/socketserver.c
#	src/logger.c\
	src/queue.c

BBB_SOURCES=src/tempsensor.c\
	src/my_i2c.c\
	src/lightsensor.c\
	src/sensortask.c

HOST_SOURCES=src/dummydata.c

TEST_SOURCES=test/sensor_test.c\
#	test/simple_test.c

CLIENT_SOURCES=src/socketclient.c

MOCKED_FUNCTIONS=
#	test/bd_queue_test.c	


