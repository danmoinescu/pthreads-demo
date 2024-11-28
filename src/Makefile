sources = \
          dispatcher.c \
          pthreads-demo.c \
          worker.c

headers = pthreads-demo.h
objects = ${sources:.c=.o}
CFLAGS = -O2 -Wall -Werror
LDFLAGS = -pthread

binary=pthreads-demo

.PHONY: clean all
all: $(binary)


$(binary): $(objects)
#	The following is applied automatically
#	$(CC) -o $@ $? $(LDFLAGS)


.c.o: $(headers)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(binary) $(objects)
