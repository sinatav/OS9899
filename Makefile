ccflags-y := -std=gnu99
TARGET_MODULE:=tools

obj-m := $(TARGET_MODULE).o

KERNEL_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true>/dev/null
