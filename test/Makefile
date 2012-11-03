TOP=..

vpath %.c $(TOP)/src

PROGRAMS = test-dlist test-bst

CFLAGS += -g -I $(TOP)/include

test-dlist-OBJS = test-dlist.o
test-bst-OBJS = test-bst.o bst.o

include $(TOP)/include/common.mk

run-%: %
	./$<

.PHONY: run-tests
run-tests: $(patsubst %,run-%,$(PROGRAMS))
