# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mnitominc
HEADERS  = $(PROGS:=.h)
CDEFINES = #                        cpp defines
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
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) : $(HEADERS)

$(LINT_LIST) : $(HEADERS)

# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(CC_PROG_LIB) $(CC_MINC_LIB)
	$(CC) $@.o -o $@ $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_PROG_LIB) $(LINT_MINC_LIB)
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_PROG_LIB) $(LINT_MINC_LIB)

# Remove all derived files in this directory
mostlyclean clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ)

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)
