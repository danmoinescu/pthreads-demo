sources = \
          dispatcher.c \
          pthreads-demo.c \
          worker.c
baseline_sources = \
          baseline.c

headers = pthreads-demo.h work.h
objects = ${sources:.c=.o}
baseline_objects = ${baseline_sources:.c=.o}
CFLAGS = -O2 -Wall -Werror
LDFLAGS = -pthread

binary=pthreads-demo
baseline_binary=baseline

.PHONY: clean all
all: $(binary) $(baseline_binary)


$(binary): $(objects)
#	The following implicit rule is applied automatically
#	$(CC) -o $@ $? $(LDFLAGS)


$(baseline_binary): $(baseline_objects)
#	The following implicit rule is applied automatically
#	$(CC) -o $@ $? $(LDFLAGS)


%.o : %.c $(headers)
#	The following implicit rule is applied automatically
#	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(binary) $(baseline_binary) $(objects) $(baseline_objects)
