# --------------------------------------------------------------------
#
# MINC Makefile
#

# Executable names
PROGS    = minc_ftest
EXTRA_OBJS = minc_jackets.o
LIB_DIR  = ../src#                         directory for library
LIBRARY  = minc#                           library
CDEFINES = -D__STDC__#                        cpp defines
OPT      = -prototypes -O#        compiler options
LDOPT    = -L/usr/local/lib -lnetcdf -lsun # ld options
INCLUDES = -I. -I../proglib -I../src -I/usr/local/include
NETCDF_DIR = netcdf
OS_TYPE = irix

# For cleaning up
RM       = rm
RM_FLAGS = -f

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O
FFLAGS    = $(INCLUDES) -O
PROG_OBJ  = $(PROGS:=.o)#                 list of objects
CC_LIB    = lib$(LIBRARY).a#              C library file
LINT_EXTRA= $(EXTRA_OBJS:.o=.ln)
LINT_LIB  = llib-l$(LIBRARY).ln#          lint library file

# --------------------------------------------------------------------

.SUFFIXES: .ln#                           tell make to watch for .ln's

default: $(PROGS)

#Dependency on Makefile
minc_jackets.c $(PROG_OBJ) $(EXTRA_OBJS) $(LINT_EXTRA) : Makefile

all: $(PROGS) lint

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

.f.o:#                                    defines the rule for creating .o
	$(FC) $(FFLAGS) -c $< -o $@

# How to make executables
$(PROGS) : $$@.o $(EXTRA_OBJS) $(LIB_DIR)/$(CC_LIB)
	$(FC) -o $@ $@.o $(EXTRA_OBJS) $(FFLAGS) -L$(LIB_DIR) -l$(LIBRARY) $(LDOPT)

minc_jackets.c: $(NETCDF_DIR)/fortc1.sed $(NETCDF_DIR)/fortc2.sed \
                $(NETCDF_DIR)/common.m4 minc_jackets.src \
                $(NETCDF_DIR)/$(OS_TYPE).m4
	$(NETCDF_DIR)/fortc -L $(NETCDF_DIR) -O $(OS_TYPE) minc_jackets.src > $@

#  how to lint the executable source
lint: $(LINT_EXTRA)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_EXTRA) $(EXTRA_OBJS) $(PROGS) $(PROG_OBJ) \
         minc_jackets.c

