# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# --------------------------------------------------------------------

default : build

all : build

build : $(CC_PROG_LIB)
	@for dir in [a-z]*; \
	   do if [ -d $$dir -a $$dir != RCS ]; \
	   then \
	      cd $$dir; \
	      echo Doing make in $$dir; \
	      $(MAKE); \
	      cd ..; \
	   fi; \
	 done

$(CC_PROG_LIB):
	cd $(PROG_LIB_DIR) ; $(MAKE)

clean : 
	@for dir in *; \
	   do if [ -d $$dir -a $$dir != RCS ]; \
	   then \
	      cd $$dir; \
	      echo Doing make clean in $$dir; \
	      $(MAKE) clean; \
	      cd ..; \
	   fi; \
	 done
