# Project1 ECEN5013-002
Project 1 for Advanced Principles of Embedded Software ECEN5013-002, Spring 2018

## Run Instructions
* `make run PLATFORM=BBB` for building for BBG/BBB and transferring the executable to /home/proj1 on the BBB/BBG
* `make test` to run unittests
* `make build` or `make run` for build on host.     run builds and executes the code, build just builds the executable. This option removes I2C sensor files and replaces the functions with pseudo random value generator
* `make client` to build the external socket request client
* Run `make clean` before changing platforms
* Run `./project1 -f logfilename` to provide a Log file name, if parameters are not proper filename defaults to project1logfile.log






