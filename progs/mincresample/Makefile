# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mincresample
EXTRA_OBJS = resample_volumes.o
HEADERS  = $(PROGS:=.h)
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

