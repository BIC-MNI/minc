# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = rawtominc
EXTRA_OBJS = 
HEADERS  = 
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

