SHELL = /bin/sh
CFLAGS = -O
# LDFLAGS = -lc_s

default:
	$(MAKE) `ls *.c |sed 's/\.c$$//'`

clean:
	rm -f `ls *.c |sed 's/\.c$$//'`

.c:
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
