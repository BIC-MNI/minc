# temporary makefile

CPPFLAGS = -I/localhome/bert/projects/junk/minc/libsrc -I/localhome/bert/include
LDFLAGS = -L/localhome/bert/projects/junk/minc/.libs -L/localhome/bert/lib

LIBS = -lminc -lnetcdf -lhdf5

all: dimension-test grpattr-test datatype-test label-test volprops-test \
     hyper-test slice-test valid-test convert-test

# convert.c
convert-test-objs = convert-test.o m2util.o volume.o free.o

convert-test: $(convert-test-objs)
	gcc $(LDFLAGS) -Wall -g -o convert-test $(convert-test-objs) $(LIBS)

convert-test.o: convert.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o convert-test.o convert.c

# slice.c
slice-test-objs = slice-test.o m2util.o free.o volume.o

slice-test: $(slice-test-objs)
	gcc $(LDFLAGS) -Wall -g -o slice-test $(slice-test-objs) $(LIBS)

slice-test.o: slice.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o slice-test.o slice.c

# hyper.c
hyper-test-objs = hyper-test.o m2util.o free.o volume.o

hyper-test: $(hyper-test-objs)
	gcc $(LDFLAGS) -Wall -g -o hyper-test $(hyper-test-objs) $(LIBS)

hyper-test.o: hyper.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o hyper-test.o hyper.c

# dimension.c
dimension-test-objs = dimension-test.o m2util.o free.o

dimension-test: $(dimension-test-objs)
	gcc $(LDFLAGS) -Wall -g -o dimension-test $(dimension-test-objs) $(LIBS)

dimension-test.o: dimension.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o dimension-test.o dimension.c

dimension.o: dimension.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o dimension.o dimension.c

# grpattr.c
grpattr-test-objs = grpattr-test.o m2util.o volume.o

grpattr-test: $(grpattr-test-objs)
	gcc $(LDFLAGS) -Wall -g -o grpattr-test $(grpattr-test-objs) $(LIBS)

grpattr-test.o: grpattr.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o grpattr-test.o grpattr.c

grpattr.o: grpattr.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o grpattr.o grpattr.c

# datatype.c
datatype-test-objs = datatype-test.o grpattr.o m2util.o free.o volume.o

datatype-test: $(datatype-test-objs)
	gcc $(LDFLAGS) -Wall -g -o datatype-test $(datatype-test-objs) $(LIBS)

datatype-test.o: datatype.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o datatype-test.o datatype.c

# label.c
label-test-objs = label-test.o m2util.o free.o volume.o

label-test: $(label-test-objs)
	gcc $(LDFLAGS) -Wall -g -o label-test $(label-test-objs) $(LIBS)

label-test.o: label.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o label-test.o label.c

# volprops.c
volprops-test-objs = volprops-test.o m2util.o free.o volume.o

volprops-test: $(volprops-test-objs)
	gcc $(LDFLAGS) -Wall -g -o volprops-test $(volprops-test-objs) $(LIBS)

volprops-test.o: volprops.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o volprops-test.o volprops.c

valid-test-objs = valid-test.o m2util.o volume.o free.o

valid-test: $(valid-test-objs)
	gcc $(LDFLAGS) -Wall -g -o valid-test $(valid-test-objs) $(LIBS)

valid-test.o: valid.c minc2.h minc2_private.h
	gcc -DM2_TEST $(CPPFLAGS) -Wall -g -c -o valid-test.o valid.c

valid.o: valid.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o valid.o valid.c

volume.o: volume.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o volume.o volume.c

m2util.o: m2util.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o m2util.o m2util.c

free.o: free.c minc2.h minc2_private.h
	gcc $(CPPFLAGS) -Wall -g -c -o free.o free.c



