PROGS = acr_test dump_acr_nema extract_acr_nema read_acr_nema dicom_test \
			sample_dicom_client copy_acr_nema
HEADERS = acr_io.h dicom_client_routines.h dicom_network.h \
			element.h file_io.h group.h message.h value_repr.h
OBJS = $(PROGS:=.o)
EXTR_OBJS = $(HEADERS:.h=.o) globals.o

CC=gcc

OPTIONS = -O3
INCLUDES = -I. -I/usr/local/include
CFLAGS = $(OPTIONS) $(INCLUDES)

LDINCLUDES = -L.
LDLIBS =                    
LDOPTS = $(LDINCLUDES) $(LDLIBS)


all: $(EXTR_OBJS) $(PROGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGS): $(OBJS)
	$(CC) $< $(EXTR_OBJS) -o $@ $(OPTIONS) $(LDOPTS)

clean:
	rm -f *.o *~ $(PROGS)
