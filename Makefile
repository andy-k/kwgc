all: kwgc kwgdbg

remake: clean all

clean:
	rm -fv kwgc kwgdbg

CFLAGS=-std=gnu17 -O3 -Wall -Wextra -Wsign-conversion -pedantic -march=native -g

kwgc: kwgc.c generic_vec.c generic_khm.c tiles.c
	$(CC) $(CFLAGS) -o $@ $<
kwgdbg: kwgdbg.c
	$(CC) $(CFLAGS) -o $@ $<
