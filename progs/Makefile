# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# --------------------------------------------------------------------

default : build

all build clean : 
	$(MAKE) 'TARGET=$@' subdirs

subdirs : 
	@for dir in $(PROG_LIB_DIR) [a-z]*; \
	   do if [ -d $$dir -a $$dir != RCS ]; \
	   then \
	      cd $$dir; \
	      echo Making $(TARGET) in $$dir; \
	      $(MAKE) $(TARGET); \
	      cd ..; \
	   fi; \
	 done

