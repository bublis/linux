obj-m	:= e1000_main.o

KERNELDIR ?= ~/linux-3.17.4/
pwd	  := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	em -rf *.o *~ .depend .*.cmd *.ko *.mod.c .tmp_versions

