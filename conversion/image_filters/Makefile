SHELL = /bin/sh
CFLAGS = -O
LDFLAGS =

default:
	$(MAKE) `ls *.c |sed 's/\.c$$//'`

clean:
	rm -f `ls *.c |sed 's/\.c$$//'`

.c:
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
