LIBS = -lminc2 -lminc -lnetcdf -lhdf5 -lm
LDFLAGS = -L.. -L/localhome/bert/lib
CPPFLAGS = -I.. -I/localhome/bert/include -g -Wall

EXE=	convert-test \
	datatype-test \
	dimension-test \
	full-test \
	grpattr-test \
	hyper-test \
	label-test \
	m2stats \
	record-test \
	slice-test \
	valid-test \
	volprops-test \
	multires-test

all: $(EXE)

$(EXE): ../libminc2.a

convert-test: convert-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

datatype-test: datatype-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

dimension-test: dimension-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

full-test: full-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

grpattr-test: grpattr-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

hyper-test: hyper-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

label-test: label-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

multires-test: multires-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

record-test: record-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

slice-test: slice-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

valid-test: valid-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

volprops-test: volprops-test.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

m2stats: m2stats.o
	$(CC) $(LDFLAGS) -Wall -g -o $@ $< $(LIBS)

clean:
	-rm $(EXE)
	-rm *.o
