# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = .
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

BUILD_SUBDIRS = libsrc $(FORTRAN_SUBDIR) test doc volume_io progs

TEST_SUBDIRS = test $(FORTRAN_SUBDIR)

# --------------------------------------------------------------------

default : build runtest

all build clean mostlyclean install:
	$(MAKE) 'TARGET=$@' 'SUBDIRS=$(BUILD_SUBDIRS)' subdirs

runtest :
	$(MAKE) 'TARGET=test' 'SUBDIRS=$(TEST_SUBDIRS)' subdirs

check : runtest

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

install: installdirs

installdirs:
	@for dir in $(INSTALL_BINDIR) $(INSTALL_LIBDIR) $(INSTALL_INCDIR) \
      $(INSTALL_MANDIR)1 $(INSTALL_MANDIR)3 $(INSTALL_VOLIO_INCDIR) ; do \
      if test ! -d $$dir; \
      then echo Creating directory $$dir;mkdir -p $$dir; \
      fi; done

distclean : clean configclean

configclean :
	$(RM) $(RM_FLAGS) Make_machine_specific Make_configuration \
	      config.cache config.log config.status

