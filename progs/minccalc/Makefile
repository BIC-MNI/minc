# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

LEX= flex
YACC= bison -y
YFLAGS= -d -t

.PRECIOUS: gram.c

# Executable names
PROGS    = minccalc
EXTRA_OBJS = vector.o scalar.o eval.o ident.o sym.o node.o gram.o lex.o optim.o 
HEADERS  = node.h errx.h
CDEFINES = -DDEBUG                 # cpp defines
LDOPT    = $(PROG_LDOPT)
MANSECT  = 1
MANPAGES = $(PROGS).$(MANSECT)

include $(ROOT)/progs/Make_progs

# Make sure that intermediate .c files get saved so that we can distribute
# these. (Hopefully no need to have a adequate versions of flex and bison
# installed when only compiling.)
lex.o: lex.c

gram.o: gram.c

