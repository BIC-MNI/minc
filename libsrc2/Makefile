# temporary makefile
CC = gcc

OBJS = \
	free.o \
	m2util.o \
	volprops.o \
	volume.o \
	convert.o \
	datatype.o \
	dimension.o \
	grpattr.o \
	hyper.o \
	label.o \
	slice.o \
	valid.o \
	record.o

LIBS = -lminc -lnetcdf -lhdf5 -lm

all:	libminc2.a

libminc2.a: $(OBJS)
	ar r libminc2.a $(OBJS)
	ranlib libminc2.a

clean:
	-rm *.o
	-rm $(EXES)

convert.o: convert.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o convert.o convert.c

slice.o: slice.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o slice.o slice.c

hyper.o: hyper.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o hyper.o hyper.c

dimension.o: dimension.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o dimension.o dimension.c

grpattr.o: grpattr.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o grpattr.o grpattr.c

datatype.o: datatype.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o datatype.o datatype.c

label.o: label.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o label.o label.c

volprops.o: volprops.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o volprops.o volprops.c

valid.o: valid.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o valid.o valid.c

record.o: record.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o record.o record.c

volume.o: volume.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o volume.o volume.c

m2util.o: m2util.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o m2util.o m2util.c

free.o: free.c minc2.h minc2_private.h
	$(CC) $(CPPFLAGS) -Wall -g -c -o free.o free.c



