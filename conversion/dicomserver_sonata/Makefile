# --------------------------------------------------------------------
#
# $Id: Makefile,v 1.6 2003-08-15 20:07:16 leili Exp $
#
# $Log: Makefile,v $
# Revision 1.6  2003-08-15 20:07:16  leili
# testing CVS again
#
#
#
#
# MINC Makefile
# Written by leili Nov. 2002
#

ROOT = ../../minc
ACR_LIB_DIR = ../Acr_nema
include $(ACR_LIB_DIR)/Make_acrdefs
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
# Couple of things have been added by Leili on April 30, 2003
# dicomserver-nondebug-test , save_transferred_object_test.o
PROGS    = dicomserver-nondebug dcm2mnc dicomserver-debug dicomserver-nondebug2 dicomserver-nondebug-old
EXTRA_OBJS = dicom_element_defs.o open_connection.o reply.o \
             save_transferred_object.o use_the_files.o \
             siemens_dicom_to_minc.o siemens_dicom_read.o minc_file.o \
             string_to_filename.o project_file.o \
             parse_dicom_groups.o siemens_to_dicom.o progress.o
HEADERS  = dicomserver.h dicom_prototypes.h dicom_element_defs.h \
           spi_element_defs.h siemens_dicom_to_minc.h
CDEFINES = -DDEBUG#                        cpp defines
INCLUDES = -I/usr/include -I. -I$(ACR_LIB_DIR) -I$(PROG_LIB_DIR) \
	            -I$(VOLIO_LIB_DIR)/Include -Isiemens_include \
	            -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)
LDOPT    = $(CC_ACR_LIB) $(PROG_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

dicomserver-debug.o: dicomserver-nondebug.c
dicomserver-nondebug.o: dicomserver-nondebug.c
#Added by Leili
dicomserver-nondebug2.o: dicomserver-nondebug2.c
dicomserver-nondebug-old.o: dicomserver-nondebug-old.c
