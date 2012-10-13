TOP=..

PROGRAMS = test-dlist

CFLAGS += -g -I $(TOP)/include

test-dlist-OBJS = test-dlist.o

include $(TOP)/include/common.mk

run-%: %
	./$<

.PHONY: run-tests
run-tests: $(patsubst %,run-%,$(PROGRAMS))
