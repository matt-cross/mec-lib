TOP=..

vpath %.c $(TOP)/src

PROGRAMS = test-dlist test-bst test-crc

CFLAGS += -g -I $(TOP)/include -std=gnu99 -Wall -Werror

test-dlist-OBJS = test-dlist.o
test-bst-OBJS = test-bst.o bst.o
test-crc-OBJS = test-crc.o crc.o

include $(TOP)/include/common.mk

run-%: %
	./$<

.PHONY: run-tests
run-tests: $(patsubst %,run-%,$(PROGRAMS))
