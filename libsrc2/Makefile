# temporary makefile

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

all:	libminc2.a test

test:	full-test \
	dimension-test \
	grpattr-test \
	datatype-test \
	label-test \
	volprops-test \
	hyper-test \
	slice-test \
	valid-test \
	convert-test \
	record-test

libminc2.a: $(OBJS)
	ar r libminc2.a $(OBJS)
	ranlib libminc2.a

#full-test
full-test-objs = full-test.o libminc2.a

full-test: $(full-test-objs)
	gcc $(LDFLAGS) -Wall -g -o full-test $(full-test-objs) $(LIBS)

full-test.o: full-test.c minc2.h 
	gcc $(CPPFLAGS) -Wall -g -c -o full-test.o full-test.c

#convert-test

# convert.c
convert-test-objs = convert-test.o libminc2.a

convert-test: $(convert-test-objs)
	gcc $(LDFLAGS) -Wall -g -o convert-test $(convert-test-objs) $(LIBS)
convert-test.o: convert.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o convert-test.o convert.c

convert.o: convert.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o convert.o convert.c
# slice.c

slice-test-objs = slice-test.o libminc2.a

slice-test: $(slice-test-objs)
	gcc $(LDFLAGS) -Wall -g -o slice-test $(slice-test-objs) $(LIBS)

slice-test.o: slice.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o slice-test.o slice.c

slice.o: slice.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o slice.o slice.c
# hyper.c

hyper-test-objs = hyper-test.o libminc2.a


hyper-test: $(hyper-test-objs)
	gcc $(LDFLAGS) -Wall -g -o hyper-test $(hyper-test-objs) $(LIBS)

hyper-test.o: hyper.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o hyper-test.o hyper.c

hyper.o: hyper.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o hyper.o hyper.c

# dimension.c
dimension-test-objs = dimension-test.o libminc2.a

dimension-test: $(dimension-test-objs)
	gcc $(LDFLAGS) -Wall -g -o dimension-test $(dimension-test-objs) $(LIBS)

dimension-test.o: dimension.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o dimension-test.o dimension.c

dimension.o: dimension.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o dimension.o dimension.c

# grpattr.c

grpattr-test-objs = grpattr-test.o libminc2.a

grpattr-test: $(grpattr-test-objs)
	gcc $(LDFLAGS) -Wall -g -o grpattr-test $(grpattr-test-objs) $(LIBS)

grpattr-test.o: grpattr.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o grpattr-test.o grpattr.c

grpattr.o: grpattr.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o grpattr.o grpattr.c

# datatype.c

datatype-test-objs = datatype-test.o libminc2.a

datatype-test: $(datatype-test-objs)
	gcc $(LDFLAGS) -Wall -g -o datatype-test $(datatype-test-objs) $(LIBS)

datatype-test.o: datatype.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o datatype-test.o datatype.c

datatype.o: datatype.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o datatype.o datatype.c

# label.c

label-test-objs = label-test.o libminc2.a


label-test: $(label-test-objs)
	gcc $(LDFLAGS) -Wall -g -o label-test $(label-test-objs) $(LIBS)

label-test.o: label.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o label-test.o label.c

label.o: label.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o label.o label.c

# volprops.c
volprops-test-objs = volprops-test.o libminc2.a

volprops-test: $(volprops-test-objs)
	gcc $(LDFLAGS) -Wall -g -o volprops-test $(volprops-test-objs) $(LIBS)

volprops-test.o: volprops.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o volprops-test.o volprops.c

volprops.o: volprops.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o volprops.o volprops.c


valid-test-objs = valid-test.o libminc2.a


valid-test: $(valid-test-objs)
	gcc $(LDFLAGS) -Wall -g -o valid-test $(valid-test-objs) $(LIBS)

valid-test.o: valid.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o valid-test.o valid.c

valid.o: valid.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o valid.o valid.c

record-test-objs = record-test.o libminc2.a

record-test: $(record-test-objs)
	gcc $(LDFLAGS) -Wall -g -o record-test $(record-test-objs) $(LIBS)

record-test.o: record.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o record-test.o record.c

record.o: record.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o record.o record.c

volume.o: volume.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o volume.o volume.c

m2util.o: m2util.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o m2util.o m2util.c -lm -lhdf5

free.o: free.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o free.o free.c



