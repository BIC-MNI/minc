# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = .
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

BUILD_SUBDIRS = src $(FORTRAN_SUBDIR) test doc volume_io progs

TEST_SUBDIRS = test $(FORTRAN_SUBDIR)

# --------------------------------------------------------------------

default : build runtest

all build clean install:
	$(MAKE) 'TARGET=$@' 'SUBDIRS=$(BUILD_SUBDIRS)' subdirs

runtest :
	$(MAKE) 'TARGET=test' 'SUBDIRS=$(TEST_SUBDIRS)' subdirs

subdirs :
	@for dir in $(SUBDIRS); \
	   do if [ -d $$dir ]; \
	   then \
	      cd $$dir; \
	      echo Making $(TARGET) in $$dir; \
	      $(MAKE) $(TARGET); \
	      cd ..; \
	   fi; \
	 done
