# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
PROGS    = minctoraw
HEADERS  = 
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(PROG_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint

# --------------------------------------------------------------------

default: build

all: build lint

build: $(PROGS)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) : $(HEADERS)

$(LINT_LIST) : $(HEADERS)

# How to make executables
$(PROGS) : $$@.o $(CC_PROG_LIB) $(CC_MINC_LIB)
	$(CC) $@.o -o $@ $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $$@ln $(LINT_PROG_LIB) $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_PROG_LIB) $(LINT_MINC_LIB)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ)

install: build
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)

