# --------------------------------------------------------------------
#
# MINC Makefile
#

# Executable names
PROGS    = minc_ftest
JACKETS_SRC  = minc_jackets.src
LIB_DIR  = ../src#                         directory for library
LIBRARY  = minc#                           library
CDEFINES = -D__STDC__#                        cpp defines
OPT      = -prototypes -O#        compiler options
LDOPT    = -L/usr/local/lib -lnetcdf -lsun # ld options
INCLUDES = -I. -I../src -I/usr/local/include
NETCDF_DIR = netcdf
OS_TYPE = irix

# For cleaning up
RM       = rm
RM_FLAGS = -f
AR       = ar
AR_FLAGS = -r
RANLIB   = true

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O
FFLAGS    = $(INCLUDES) -O
JACKETS_C = $(JACKETS_SRC:.src=.c)
JACKETS_OBJ = $(JACKETS_SRC:.src=.o)
JACKETS_LN = $(JACKETS_SRC:.src=.ln)
PROG_OBJ  = $(PROGS:=.o)#                 list of objects
CC_LIB    = lib$(LIBRARY).a#              C library file
LINT_LIB  = llib-l$(LIBRARY).ln#          lint library file

# --------------------------------------------------------------------

.SUFFIXES: .ln#                           tell make to watch for .ln's

default: $(PROGS)

#Dependency on Makefile
$(JACKETS_C) $(JACKETS_OBJ) $(JACKETS_LN) $(PROG_OBJ) : Makefile

all: $(PROGS) lint library

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

.f.o:#                                    defines the rule for creating .o
	$(FC) $(FFLAGS) -c $< -o $@

# How to make executables
$(PROGS) : $$@.o $(JACKETS_OBJ) $(LIB_DIR)/$(CC_LIB)
	$(FC) -o $@ $@.o $(JACKETS_OBJ) $(FFLAGS) -L$(LIB_DIR) -l$(LIBRARY) $(LDOPT)

$(JACKETS_C): $(JACKETS_SRC) \
              $(NETCDF_DIR)/fortc1.sed $(NETCDF_DIR)/fortc2.sed \
              $(NETCDF_DIR)/common.m4 $(NETCDF_DIR)/$(OS_TYPE).m4
	$(NETCDF_DIR)/fortc -L $(NETCDF_DIR) -O $(OS_TYPE) $(JACKETS_SRC) > $@

#  how to lint the executable source
lint: $(JACKETS_LN)

# name of library
library: $(LIB_DIR)/$(CC_LIB)

# how to update library
$(LIB_DIR)/$(CC_LIB): $(JACKETS_OBJ)
	$(AR) $(AR_FLAGS) $(LIB_DIR)/$(CC_LIB) $(JACKETS_OBJ)
	$(RANLIB) $(LIB_DIR)/$(CC_LIB)


# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(JACKETS_C) $(JACKETS_OBJ) $(JACKETS_LN) \
                     $(PROGS) $(PROG_OBJ)

