# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = scxtominc scxmnem
EXTRA_OBJS = scx_file.o insertblood.o
HEADERS  = scx_file.h
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

