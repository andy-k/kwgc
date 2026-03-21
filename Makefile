all: kwgc kwgdbg kbwgdbg

remake: clean all

clean:
	rm -fv kwgc kwgdbg kbwgdbg

CFLAGS=-std=gnu17 -O3 -Wall -Wextra -Wsign-conversion -pedantic -march=native -g

kwgc: kwgc.c alloc.c generic_vec.c generic_khm.c generic_xwg_read.c tiles.c
	$(CC) $(CFLAGS) -o $@ $<
kwgdbg: kwgdbg.c alloc.c generic_vec.c generic_xwgdbg.c
	$(CC) $(CFLAGS) -o $@ $<
kbwgdbg: kbwgdbg.c alloc.c generic_vec.c generic_xwgdbg.c
	$(CC) $(CFLAGS) -o $@ $<
