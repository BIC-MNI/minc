# --------------------------------------------------------------------
#
# MINC Makefile
#

# Object files, header files and library archive
OBJS = minc_globdef.o minc_error.o value_conversion.o netcdf_convenience.o \
	    minc_convenience.o image_conversion.o dim_conversion.o
HEADERS = minc.h minc_routines.h minc_varlists.h minc_basic.h \
	       minc_structures.h minc_private.h minc_useful.h type_limits.h
LIBRARY = minc

# Compiler and loader options
CDEFINES = -DDEBUG
OPT      = -O
LDOPT    = -L/usr/local/lib -lnetcdf -lsun -lm -lc_s
INCLUDES = -I. -I/usr/local/include

# Options for creating library
RM       = rm
RM_FLAGS = -f
AR       = ar
AR_FLAGS = -r
RANLIB   = true

# --------------------------------------------------------------------

# CFLAGS and LINTFLAGS should be same, except for -g/-O
CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)
LINTFLAGS = $(CDEFINES) $(INCLUDES)

# C library file
CC_LIB    = lib$(LIBRARY).a

# Lint files
LINT_LIST = $(OBJS:.o=.ln)
LINT_LIB  = llib-l$(LIBRARY).ln

# --------------------------------------------------------------------

.SUFFIXES: .ln#                           tell make to watch for .ln's

default : $(CC_LIB)

all : $(CC_LIB) $(LINT_LIB)

# define the rule for creating .ln
$(LINT_LIST) : $(HEADERS)
.c.ln:
	lint $(LINTFLAGS) -c $< -o $@

# define the rule for creating .o
$(OBJS) : $(HEADERS)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# How to create library
$(CC_LIB): $(OBJS)
	$(RM) $(RM_FLAGS) $(CC_LIB)
	$(AR) $(AR_FLAGS) $(CC_LIB) $(OBJS)
	$(RANLIB) $(CC_LIB)

# Lint everything
lint: $(LINT_LIB)

$(LINT_LIB) : $(LINT_LIST)
	lint -o $(LIBRARY) -x $(LINTFLAGS) $(LINT_LIST)

# Remove all derived files
clean:
	\rm -f   $(OBJS) $(LINT_LIST) $(CC_LIB) $(LINT_LIB)
