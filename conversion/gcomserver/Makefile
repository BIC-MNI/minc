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
PROGS    = gcomserver gcomserver-debug
TOMINC_OBJS = spi_element_defs.o gyro_to_minc.o gyro_read.o minc_file.o \
              string_to_filename.o 
EXTRA_OBJS = open_connection.o reply.o spi_message.o \
             save_transferred_object.o use_the_files.o \
             project_file.o convert_to_dicom.o $(TOMINC_OBJS)
HEADERS  = gcomserver.h gcom_prototypes.h spi_element_defs.h gyro_to_minc.h
OPT      = -O
CDEFINES = -DDEBUG#                        cpp defines
INCLUDES = -I/usr/include -I. -I$(ACR_LIB_DIR) -I$(PROG_LIB_DIR) \
	            -I$(VOLIO_LIB_DIR)/Include \
	            -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)
TOMINC_LDOPT = $(CC_ACR_LIB) $(PROG_LDOPT)
LDOPT    = -ldn $(TOMINC_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

build: gyrotominc

lint: gyrotominc.

gyrotominc.o gyrotominc.ln : Makefile $(HEADERS)

gcomserver-debug.o: gcomserver.c

gyrotominc : gyrotominc.o $(TOMINC_OBJS)
	$(CC) $@.o -o $@ $(TOMINC_OBJS) $(CC_ACR_LIB) $(PROG_LDOPT)

gyrotominc. : gyrotominc.ln
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_EXTRA) \
	      $(LINT_PROG_LIB) $(LINT_MINC_LIB)

install_gyrotominc: gyrotominc
	$(CP) $(CP_FLAGS) gyrotominc $(INSTALL_BINDIR)/.
	$(CHMOD) $(CHMOD_XFLAGS) $(INSTALL_BINDIR)/gyrotominc

install: install_gyrotominc

mostlyclean clean: clean_gyrotominc

clean_gyrotominc:
	$(RM) $(RM_FLAGS) gyrotominc gyrotominc.o gyrotominc.ln
