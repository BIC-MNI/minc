# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

LEX= flex
YFLAGS= -d -t

.PRECIOUS: gram.c

# Executable names
PROGS    = minccalc
EXTRA_OBJS = vector.o eval.o ident.o sym.o node.o gram.o lex.o optim.o 
HEADERS  = node.h errx.h
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

