# Target library
lib := libuthread.a
objs :=  queue.o uthread.o context.o sem.o preempt.o

CC	= gcc
CFLAGS  := -Wall -Wextra -Werror -o -MMD -std=gnu99
CFLAGS  += -g

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

all: $(lib)
## TODO: Phase 1

%.a: $(objs)
	ar rcs $@ $^

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(Q)rm -rf $(objs) $(deps) $(lib)