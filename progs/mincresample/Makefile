# --------------------------------------------------------------------
#
# MINC Makefile
#

# Executable names
PROGS    = mincresample
EXTRA_OBJS = resample_volumes.o
HEADERS  = $(PROGS:=.h)
PROG_LIB_DIR = ../proglib
PROG_LIB  = mincprog
LIB_DIR  = ../../src#                         directory for library
LIBRARY  = minc#                           library
CDEFINES = -D__STDC__#                        cpp defines
OPT      = -fullwarn -prototypes -g#        compiler options
LDOPT    = -L/usr/local/lib -lnetcdf -lsun -lrecipes -lm -lc_s# ld options
INCLUDES = -I. -I../proglib -I../../src -I/usr/local/include

# For cleaning up
RM       = rm
RM_FLAGS = -f

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
CC_LIB    = lib$(LIBRARY).a#              C library file
CC_PROG_LIB= $(PROG_LIB_DIR)/lib$(PROG_LIB).a# library for prog objs
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_EXTRA= $(EXTRA_OBJS:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint
LINT_LIB  = llib-l$(LIBRARY).ln#          lint library file
LINT_PROG_LIB = $(PROG_LIB_DIR)/llib-l$(PROG_LIB).ln#

# --------------------------------------------------------------------

.SUFFIXES: .ln#                           tell make to watch for .ln's

default: $(PROGS)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) $(EXTRA_OBJS) $(LINT_EXTRA) : Makefile

all: $(PROGS) lint

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) $(EXTRA_OBJS) : $(HEADERS)

$(LINT_LIST) $(LINT_EXTRA) : $(HEADERS)

# How to make executables
$(PROGS) : $$@.o $(EXTRA_OBJS) $(CC_PROG_LIB) $(LIB_DIR)/$(CC_LIB)
	$(CC) -o $@ $@.o $(EXTRA_OBJS) $(CC_PROG_LIB) -L$(LIB_DIR) -l$(LIBRARY) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $$@ln $(LINT_EXTRA) $(LINT_PROG_LIB) $(LIB_DIR)/$(LINT_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_EXTRA) $(LINT_PROG_LIB) $(LIB_DIR)/$(LINT_LIB)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ)

