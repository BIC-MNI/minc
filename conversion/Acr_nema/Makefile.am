AUTOMAKE_OPTIONS = check-news

# This is the only header installed in the "includedir"
#
include_HEADERS = acr_nema.h

# All other headers are installed in this subdirectory.
#
include_acr_nemadir = $(includedir)/acr_nema

include_acr_nema_HEADERS = \
	acr_nema/acr_io.h \
	acr_nema/dicom_client_routines.h \
	acr_nema/dicom_network.h \
	acr_nema/element.h \
	acr_nema/file_io.h \
	acr_nema/group.h \
	acr_nema/message.h \
	acr_nema/value_repr.h

# Libraries which must be built and installed.
#
lib_LTLIBRARIES = libacr_nema.la

bin_PROGRAMS = \
	acr_test \
	dump_acr_nema \
	extract_acr_nema \
	read_acr_nema \
	dicom_test \
	copy_acr_nema

noinst_PROGRAMS = \
	sample_dicom_client

LDADD = libacr_nema.la

acr_test_SOURCES = acr_test.c

dump_acr_nema_SOURCES = dump_acr_nema.c

extract_acr_nema_SOURCES = extract_acr_nema.c

read_acr_nema_SOURCES = read_acr_nema.c

dicom_test_SOURCES = dicom_test.c

copy_acr_nema_SOURCES = copy_acr_nema.c

libacr_nema_la_LDFLAGS = -version-info 1:0:1
libacr_nema_la_SOURCES = \
	acr_io.c \
	dicom_client_routines.c \
	dicom_network.c \
	element.c \
	file_io.c \
	globals.c \
	group.c \
	message.c \
	value_repr.c
