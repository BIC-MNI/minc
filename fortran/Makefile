# --------------------------------------------------------------------
#
# MINC fortran Makefile
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = minc_ftest
JACKETS_SRC  = minc_jackets.src
CDEFINES = #                        cpp defines
LDOPT    = $(MINC_LDOPT:-lc_s=)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O
FFLAGS    = $(INCLUDES) -O
JACKETS_C = $(JACKETS_SRC:.src=.c)
JACKETS_OBJ = $(JACKETS_SRC:.src=.o)
JACKETS_LN = $(JACKETS_SRC:.src=.ln)
PROG_OBJ  = $(PROGS:=.o)#                 list of objects

# --------------------------------------------------------------------

default: build 

all: build lint library test

# Make sure that we build the jackets first
build : $(JACKETS_OBJ) $(PROGS) library

#Dependency on Makefile
$(JACKETS_OBJ) $(JACKETS_LN) $(PROG_OBJ) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

.f.o:#                                    defines the rule for creating .o
	$(FC) $(FFLAGS) -c $< -o $@

# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(CC_MINC_LIB)
	$(FC) -o $@ $@.o $(JACKETS_OBJ) $(FFLAGS) $(LDOPT)

# How to run test
test: $(PROGS)
	minc_ftest

# We no longer derive the C code from the .src file since the 
# m4 interface is not used by NetCDF 3.x and beyond
#$(JACKETS_C): $(JACKETS_SRC) 
#	$(NETCDF_FORTRAN_DIR)/fortc -L $(NETCDF_FORTRAN_DIR) -O $(OS_TYPE) \
#	              $(JACKETS_SRC) > $@.new && mv $@.new $@

#  how to lint the executable source
lint: $(JACKETS_LN)

# name of library
library: $(CC_MINC_LIB)

# how to update library
$(CC_MINC_LIB): $(JACKETS_OBJ)
	$(AR) $(AR_FLAGS) $(CC_MINC_LIB) $(JACKETS_OBJ)
	$(RANLIB) $(CC_MINC_LIB)


# Remove most derived files in this directory - leave jacket obj for library
mostlyclean:
	$(RM) $(RM_FLAGS) $(JACKETS_LN) \
                     $(PROGS) $(PROG_OBJ)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(JACKETS_OBJ) $(JACKETS_LN) \
                     $(PROGS) $(PROG_OBJ)

install:
	$(CP) $(CP_FLAGS) minc.inc $(INSTALL_INCDIR)
	$(CHMOD) $(CHMOD_FLAGS) $(INSTALL_INCDIR)/minc.inc
