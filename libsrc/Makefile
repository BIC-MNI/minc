# --------------------------------------------------------------------
#
# MINC library Makefile
#

ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Object files, header files and library archive
OBJS = minc_globdef.o minc_error.o value_conversion.o netcdf_convenience.o \
	    minc_convenience.o image_conversion.o dim_conversion.o
HEADERS = minc.h minc_routines.h minc_varlists.h minc_basic.h \
	       minc_structures.h minc_private.h minc_useful.h type_limits.h

# Compiler and loader options
CDEFINES = 

# --------------------------------------------------------------------

# CFLAGS and LINTFLAGS should be same, except for -g/-O
CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)
LINTFLAGS = $(CDEFINES) $(INCLUDES)

# Lint files
LINT_LIST = $(OBJS:.o=.ln)

# --------------------------------------------------------------------

default : build

all : build $(LINT_MINC_LIB)

build : $(CC_MINC_LIB)

#Dependency on Makefile
$(OBJS) $(CC_MINC_LIB) $(LINT_LIST) $(LINT_MINC_LIB) : Makefile

# define the rule for creating .ln
$(LINT_LIST) : $(HEADERS)
.c.ln:
	lint $(LINTFLAGS) -c $< -o $@

# define the rule for creating .o
$(OBJS) : $(HEADERS)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# How to create library
$(CC_MINC_LIB): $(OBJS)
	$(RM) $(RM_FLAGS) $(CC_MINC_LIB)
	$(AR) $(AR_FLAGS) $(CC_MINC_LIB) $(OBJS)
	$(RANLIB) $(CC_MINC_LIB)

# Lint everything
lint: $(LINT_MINC_LIB)

$(LINT_MINC_LIB) : $(LINT_LIST)
	lint -o $(MINC_LIB) -x -u $(LINTFLAGS) $(LINT_LIST)

# Remove all derived files
clean:
	$(RM) $(RM_FLAGS)  $(OBJS) $(LINT_LIST) $(CC_MINC_LIB) $(LINT_MINC_LIB)
