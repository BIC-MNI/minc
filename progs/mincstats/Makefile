PROGS = mincstats
OBJS = $(PROGS:=.o)

OPTIONS = -O3

INCLUDES = -I. \
           -I/data/nil/rotor/opt/include \
           -I/usr/local/mni/include \
           -I/usr/local/include

LDINCLUDES = -L/data/nil/rotor/opt/lib32 \
             -L/software/build/external/IRIX6/lib

LDLIBS = -lParseArgv -lvolume_io -lmincprog -lminc -lnetcdf -lm

LDOPTS = $(LDINCLUDES) $(LDLIBS)

CC=cc
CFLAGS = $(OPTIONS) $(INCLUDES)

all: $(PROGS) 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGS): $(OBJS)
	$(CC) $@.o -o $@ $(OPTIONS) $(LDOPTS)

clean:
	rm -f *.o *~ $(PROGS)
