# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../../minc
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mnitominc
EXTRA_OBJS =
HEADERS  = $(PROGS:=.h)
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

