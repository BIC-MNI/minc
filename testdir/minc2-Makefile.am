INCLUDES = -I$(top_srcdir)/libsrc2

script_tests = \
	testminctools.sh

all-local:
	cd $(srcdir) && chmod +x $(script_tests)


LDADD = ../../libminc2.la

AM_CFLAGS = -DAPPARENTORDER

ALL_TESTS =	\
	create-test-images \
	create-test-images-2 \
	datatype-test \
	dimension-test \
	full-test \
	grpattr-test \
	hyper-test \
        hyper-test-2 \
	label-test \
	record-test \
	slice-test \
	valid-test \
	volprops-test \
	multires-test \
	convert-test \
	vector_dimension-test 

TESTS = \
	$(ALL_TESTS) \
	$(script_tests)

EXTRA_DIST = $(script_tests)

check_PROGRAMS = $(ALL_TESTS)

CLEANFILES = *.mnc



