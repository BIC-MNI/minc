# --------------------------------------------------------------------
#
# MINC Makefile for program library
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Object names
OBJS     = ParseArgv.o vax_conversions.o time_stamp.o nd_loop.o voxel_loop.o
MANPAGES = ParseArgv.3
CDEFINES = #                        cpp defines

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

LINT_OBJS = $(OBJS:.o=.ln)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build lint

build: $(CC_PROG_LIB) man

man: $(MANPAGES)

#Dependency on Makefile
$(OBJS) $(LINT_OBJS) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

# How to create library
$(CC_PROG_LIB): $(OBJS)
	$(RM) $(RM_FLAGS) $(CC_PROG_LIB)
	$(AR) $(AR_FLAGS) $(CC_PROG_LIB) $(OBJS)
	$(RANLIB) $(CC_PROG_LIB)

# Lint
lint : $(LINT_PROG_LIB)

# How to create lint library
$(LINT_PROG_LIB) : $(LINT_OBJS)
	$(LINT) -o $(PROG_LIB) $(LINT_LIBOPTS) $(LINTFLAGS) $(LINT_OBJS)

# how to make man pages
.man3.3:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files in this directory
clean: mostlyclean
	$(RM) $(RM_FLAGS) $(CC_PROG_LIB) $(LINT_PROG_LIB)

mostlyclean:
	$(RM) $(RM_FLAGS) $(OBJS) $(LINT_OBJS)

install: 
	@echo Nothing to install
