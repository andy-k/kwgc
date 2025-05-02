all: kwgc kwgdbg kbwgdbg

remake: clean all

clean:
	rm -fv kwgc kwgdbg kbwgdbg

CFLAGS=-std=gnu17 -O3 -Wall -Wextra -Wsign-conversion -pedantic -march=native -g

kwgc: kwgc.c generic_vec.c generic_khm.c tiles.c
	$(CC) $(CFLAGS) -o $@ $<
kwgdbg: kwgdbg.c
	$(CC) $(CFLAGS) -o $@ $<
kbwgdbg: kbwgdbg.c
	$(CC) $(CFLAGS) -o $@ $<
