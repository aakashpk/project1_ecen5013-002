COMMON_SOURCES=src/main.c\
#	src/logger.c\
	src/queue.c

BBB_SOURCES=src/tempsensor.c\
	src/my_i2c.c\
	src/lightsensor.c\
	src/sensortask.c

HOST_SOURCES=src/dummydata.c

TESTSOURCES=test/simple_test.c\
	test\sensor_test.c\
	test\bd_queue_test.c	


