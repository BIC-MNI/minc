# --------------------------------------------------------------------
#
# MINC Makefile
#

# Object names
OBJS     = ParseArgv.o vax_conversions.o
PROG_LIB  = mincprog
CDEFINES = #                        cpp defines
OPT      = -g#                             compiler options
INCLUDES = -I. # include directories

# Options for creating library
RM       = rm
RM_FLAGS = -f
AR       = ar
AR_FLAGS = -r
RANLIB   = true

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

CC_PROG_LIB= lib$(PROG_LIB).a#            library for objs
LINT_OBJS = $(OBJS:.o=.ln)
LINT_PROG_LIB = llib-l$(PROG_LIB).ln#

# --------------------------------------------------------------------

.SUFFIXES: .ln#                           tell make to watch for .ln's

default: $(CC_PROG_LIB)

#Dependency on Makefile
$(OBJS) $(LINT_OBJS) : Makefile

all: $(CC_PROG_LIB) lint

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

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
	lint -o $(PROG_LIB) -x -u $(LINTFLAGS) $(LINT_OBJS)

# Remove all derived files in this directory
clean:
	\rm -f   $(OBJS) $(LINT_OBJS)

