# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
SCRIPTS  = mincview
PROGS    = invert_raw_image
HEADERS  =
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(MACHINE_LDOPT)
MANSECT  = 1
#MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

