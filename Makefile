# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = .
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

SUBDIRS = src $(FORTRAN_SUBDIR) test doc progs

TEST_SUBDIRS = test $(FORTRAN_SUBDIR)

DUMMY = dummy_file_that_does_not_exist

# --------------------------------------------------------------------

.PRECIOUS : $(SUBDIRS)

default : build runtest

all build clean:
	@TARGET=$@; export TARGET; $(MAKE) $(SUBDIRS)

runtest :
	@TARGET=test; export TARGET; $(MAKE) $(TEST_SUBDIRS)

$(SUBDIRS) : $(DUMMY)
	@cd $@ ; echo Making $(TARGET) in $@; $(MAKE) $(TARGET)

$(DUMMY) :

