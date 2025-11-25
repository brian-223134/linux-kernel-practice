# Makefile for the linked list synchronization assignment

# [Important Rule]
# obj-m += <new_module_name>.o
# <new_module_name>-objs := <source_file1>.o <source_file2>.o ...

# 1. Spinlock Module
# Output: my_spinlock.ko
obj-m += my_spinlock.o
my_spinlock-objs := linked_list_impl_spinlock.o calclock.o

# 2. Mutex Module
# Output: my_mutex.ko
obj-m += my_mutex.o
my_mutex-objs := linked_list_impl_mutex.o calclock.o

# 3. RW Semaphore Module
# Output: my_rwsem.ko
obj-m += my_rwsem.o
my_rwsem-objs := linked_list_impl_rwsema.o calclock.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
