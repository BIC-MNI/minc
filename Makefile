# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = .
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

LIB_SUBDIRS = libsrc $(FORTRAN_SUBDIR) volume_io

PROG_SUBDIRS = progs

BUILD_SUBDIRS = $(LIB_SUBDIRS) $(PROG_SUBDIRS)

DOC_SUBDIRS = doc volume_io/Documentation

TEST_SUBDIRS = testdir $(FORTRAN_SUBDIR)

# --------------------------------------------------------------------

default : build

all build install:
	$(MAKE) 'TARGET=$@' 'SUBDIRS=$(BUILD_SUBDIRS)' subdirs

clean mostlyclean:
	$(MAKE) 'TARGET=$@' \
	   'SUBDIRS=$(BUILD_SUBDIRS) $(DOC_SUBDIRS) $(TEST_SUBDIRS)' subdirs

libs:
	$(MAKE) 'TARGET=build' 'SUBDIRS=$(LIB_SUBDIRS)' subdirs

programs:
	$(MAKE) 'TARGET=build' 'SUBDIRS=$(PROG_SUBDIRS)' subdirs

docs:
	$(MAKE) 'TARGET=build' 'SUBDIRS=$(DOC_SUBDIRS)' subdirs

install-docs:
	$(MAKE) 'TARGET=install' 'SUBDIRS=$(DOC_SUBDIRS)' subdirs

test :
	$(MAKE) 'TARGET=test' 'SUBDIRS=$(TEST_SUBDIRS)' subdirs

check : test

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

install-docs: install-doc-dirs

install-doc-dirs:
	if test ! -d $(INSTALL_DOCDIR) ; \
      then echo Creating directory $(INSTALL_DOCDIR) ; \
           mkdir -p $(INSTALL_DOCDIR); \
      fi; done

distclean : clean configclean

configclean :
	$(RM) $(RM_FLAGS) Make_machine_specific Make_configuration \
	      config.cache config.log config.status

