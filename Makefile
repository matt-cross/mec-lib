SUBDIRS=src test

all-%: %
	make -C $< all

clean-%: %
	make -C $< clean

.PHONY: all
all: $(patsubst %,all-%,$(SUBDIRS))

.PHONY: clean
clean: $(patsubst %,clean-%,$(SUBDIRS))
