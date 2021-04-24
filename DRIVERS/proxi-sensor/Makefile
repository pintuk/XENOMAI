EXTRA_CFLAGS += -I$(shell pwd)

obj-m := proxi-sensor.o

TOOLCHAIN := arm-linux-gnueabihf-

KDIR ?= 
#KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(KDIR) M=$(PWD) modules
	#$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

