# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# --------------------------------------------------------------------

default : build

all build clean install : 
	$(MAKE) 'TARGET=$@' subdirs

subdirs : 
	@for dir in $(PROG_LIB_DIR) $(ACR_LIB_DIR) [a-z]*; \
	   do if [ -d $$dir -a $$dir != RCS ]; \
	   then \
	      cd $$dir; \
	      echo Making $(TARGET) in $$dir; \
	      $(MAKE) $(TARGET); \
	      cd ..; \
	   fi; \
	 done

