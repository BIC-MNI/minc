# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
SCRIPTS  = mincview
PROGS    = invert_raw_image
MANSECT  = 1
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(MACHINE_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint
MANPAGES = #$(PROGS).$(MANSECT)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build lint

build: $(PROGS) man

man: $(MANPAGES)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

# How to make executables
$(PROGS) : $$@.o
	$(CC) -o $@ $@.o $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $$@ln
	lint -u $(LINTFLAGS) $@ln

# how to make man pages
.man1.1:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ) $(MANPAGES)
	            
install:
	$(CP) $(CP_FLAGS) $(SCRIPTS) $(PROGS) $(INSTALL_BINDIR)

