CC=gcc
CFLAGS=

cpu: cpu.c
	$(CC) -o $@ $< $(CFLAGS);

clean:
	rm -f *.o *~ cpu
