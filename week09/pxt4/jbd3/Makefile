# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the Linux JBD3 journaling routines
#

obj-m += jbd3.o

jbd3-objs := transaction.o commit.o recovery.o checkpoint.o revoke.o journal.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(CURDIR)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
#	rm -f *.mod.* *.ko *.o

