TC_PATH=/tools/AGRtools/arm/Linaro/2012.04/gcc-linaro-arm-linux-gnueabi-2012.04-20120426_linux
TC_PREFIX=arm-linux-gnueabi-

#TC_PATH=/tools/AGRtools/arm/gcc-linaro-aarch64-linux-gnu-4.9-2014.08_linux
#TC_PREFIX=aarch64-linux-gnu-

#TC_BIN=$(TC_PATH)/bin
#CC=$(TC_BIN)/$(TC_PREFIX)gcc
CFLAGS=-g
CC=/usr/bin/gcc

all: 
	$(CC) $(CFLAGS) register_access.c -o register_access 

clean:
	\rm -rf register_access
