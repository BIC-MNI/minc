# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
PROGS    = mincresample
EXTRA_OBJS = resample_volumes.o
HEADERS  = $(PROGS:=.h)
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(MINC_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_EXTRA= $(EXTRA_OBJS:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint

# --------------------------------------------------------------------

default: build

all: build lint

build: $(PROGS)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) $(EXTRA_OBJS) $(LINT_EXTRA) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) $(EXTRA_OBJS) : $(HEADERS)

$(LINT_LIST) $(LINT_EXTRA) : $(HEADERS)

# How to make executables
$(PROGS) : $$@.o $(EXTRA_OBJS) $(CC_PROG_LIB) $(CC_MINC_LIB)
	$(CC) -o $@ $@.o $(EXTRA_OBJS) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $$@ln $(LINT_EXTRA) $(LINT_PROG_LIB) $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_EXTRA) $(LINT_PROG_LIB) $(LINT_MINC_LIB)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ) \
	            $(EXTRA_OBJS) $(LINT_EXTRA)

