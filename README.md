# Project1 ECEN5013-002
Project 1 for Advanced Principles of Embedded Software ECEN5013-002, Spring 2018

## Run Instructions
* `make all` for build on host. This option removes I2C sensor files and replaces the functions with pseudo random value generator
* `make PLATFORM=BBB` for building for BBG/BBB and transferring the executable to /home/proj1 on the BBB/BBG
* `make client` to build the external socket request client
* Run `make clean` before changing platforms
* `make unittest` to run unittests


## Files in Repo
* lightsensor.c/.h
* tempsensor.c/.h
* my_i2c.c/.h
* queue.c/.h
* logger.c/.h
* dummydata.c/.h
* sensortask.c/.h
* socketserver.c/.h
* main.c

* sensor_test.c
* bd_queue_test.c



