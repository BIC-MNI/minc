# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mincaverage
EXTRA_OBJS = 
HEADERS  = 
MANSECT  = 1
CDEFINES = -DDEBUG# cpp defines
LDOPT    = $(PROG_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_EXTRA = $(EXTRA_OBJS:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint
MANPAGES = $(PROGS).$(MANSECT)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build lint

build: $(PROGS) man

man: $(MANPAGES)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) $(EXTRA_OBJS) $(LINT_EXTRA) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) $(EXTRA_OBJS) : $(HEADERS)

$(LINT_LIST) $(LINT_EXTRA) : $(HEADERS)

# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(EXTRA_OBJS) $(CC_PROG_LIB) $(CC_MINC_LIB)
	$(CC) $@.o -o $@ $(EXTRA_OBJS) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_EXTRA) $(LINT_PROG_LIB) $(LINT_MINC_LIB)
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_EXTRA) \
	      $(LINT_PROG_LIB) $(LINT_MINC_LIB)

# how to make man pages
.man1.1:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files in this directory
mostlyclean clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ) \
      $(LINT_EXTRA) $(EXTRA_OBJS) $(MANPAGES)

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)
	$(CP) $(CP_FLAGS) $(MANPAGES) $(INSTALL_MANDIR)$(MANSECT)

