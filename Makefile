# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = .
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

SUBDIRS = src $(FORTRAN_SUBDIR) test doc progs

BUILD_SUBDIRS = $(SUBDIRS)

TEST_SUBDIRS = test $(FORTRAN_SUBDIR)

CLEAN_SUBDIRS = $(BUILD_SUBDIRS)

DUMMY = dummy_file_that_doesnt_exist

# --------------------------------------------------------------------

default : build

all : build runtest

.PRECIOUS : $(SUBDIRS)

build :
	@TARGET=$@; export TARGET; $(MAKE) $(BUILD_SUBDIRS)

runtest :
	@TARGET=$@; export TARGET; $(MAKE) $(TEST_SUBDIRS)

clean :
	@TARGET=$@; export TARGET; $(MAKE) $(CLEAN_SUBDIRS)

$(SUBDIRS) : $(DUMMY)
	@cd $@ ; echo Making $(TARGET) in $@; $(MAKE) $(TARGET)

$(DUMMY) :

