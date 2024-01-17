all: kwgc kwgdbg

remake: clean all

clean:
	rm -fv kwgc kwgdbg

CFLAGS=-std=c17 -O3 -Wall -Wextra -Wsign-conversion -pedantic

kwgc: kwgc.c
	$(CC) $(CFLAGS) -o $@ $^
kwgdbg: kwgdbg.c
	$(CC) $(CFLAGS) -o $@ $^
