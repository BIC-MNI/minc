# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = gcomserver gcomserver-debug gyrotominc
EXTRA_OBJS = spi_element_defs.o open_connection.o reply.o spi_message.o \
             save_transferred_object.o use_the_files.o \
             gyro_to_minc.o gyro_read.o minc_file.o \
             string_to_filename.o project_file.o
HEADERS  = gcomserver.h gcom_prototypes.h spi_element_defs.h gyro_to_minc.h
OPT      = -O
CDEFINES = -DDEBUG#                        cpp defines
INCLUDES = -I/usr/include -I. -I$(ACR_LIB_DIR) -I$(PROG_LIB_DIR) \
	            -I$(VOLIO_LIB_DIR)/Include \
	            -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)
LDOPT    = -ldn $(CC_ACR_LIB) $(PROG_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

gcomserver-debug.o: gcomserver.c
