# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../../minc
ACR_LIB_DIR = ../Acr_nema
include $(ACR_LIB_DIR)/Make_acrdefs
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = dicomserver dicomserver-debug
EXTRA_OBJS = dicom_element_defs.o open_connection.o reply.o \
             save_transferred_object.o use_the_files.o \
             siemens_dicom_to_minc.o siemens_dicom_read.o minc_file.o \
             string_to_filename.o project_file.o
HEADERS  = dicomserver.h dicom_prototypes.h dicom_element_defs.h \
           spi_element_defs.h siemens_dicom_to_minc.h
CDEFINES = -DDEBUG#                        cpp defines
INCLUDES = -I/usr/include -I. -I$(ACR_LIB_DIR) -I$(PROG_LIB_DIR) \
	            -I$(VOLIO_LIB_DIR)/Include \
	            -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)
LDOPT    = $(CC_ACR_LIB) $(PROG_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

dicomserver-debug.o: dicomserver.c

