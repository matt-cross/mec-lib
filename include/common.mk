
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ALL_OBJS = $(foreach program,$(PROGRAMS),$($(program)-OBJS))

ALL_SRCS = $(patsubst %.o,%.c,$(ALL_OBJS))

ALL_DEPS = $(patsubst %.o,%.d,$(ALL_OBJS))

define BUILD_PROGRAM
$(1): $($(1)-OBJS)
	$(CC) $(CFLAGS) -o $$@ $$^
endef

$(foreach program,$(PROGRAMS),$(eval $(call BUILD_PROGRAM,$(program))))

all: $(PROGRAMS)

clean:
	-rm $(PROGRAMS) $(ALL_OBJS) $(ALL_DEPS)

ifneq ($(MAKECMDGOALS),clean)
-include $(ALL_DEPS)
endif
