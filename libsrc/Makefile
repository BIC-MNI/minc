# --------------------------------------------------------------------
#
# MINC library Makefile
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Object files, header files and library archive
OBJS = minc_globdef.o minc_error.o value_conversion.o netcdf_convenience.o \
	    minc_convenience.o image_conversion.o dim_conversion.o \
       nd_loop.o voxel_loop.o time_stamp.o ParseArgv.o
INSTALLED_HEADERS = minc.h nd_loop.h voxel_loop.h time_stamp.h ParseArgv.h
HEADERS = $(INSTALLED_HEADERS) minc_routines.h minc_varlists.h minc_basic.h \
	       minc_structures.h minc_private.h minc_useful.h type_limits.h

# Man pages
MANPAGES = ParseArgv.3

# Compiler and loader options
CDEFINES = 

# --------------------------------------------------------------------

# CFLAGS and LINTFLAGS should be same, except for -g/-O
CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)
LINTFLAGS = $(CDEFINES) $(INCLUDES)

# Lint files
LINT_LIST = $(OBJS:.o=.ln)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default : build

all : build $(LINT_MINC_LIB)

build : $(CC_MINC_LIB) man

man: $(MANPAGES)

#Dependency on Makefile
$(OBJS) $(CC_MINC_LIB) $(LINT_LIST) $(LINT_MINC_LIB) : Makefile

# define the rule for creating .ln
$(LINT_LIST) : $(HEADERS)
.c.ln:
	$(LINT) $(LINTFLAGS) -c $< -o $@

# define the rule for creating .o
$(OBJS) : $(HEADERS)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# How to create library
$(CC_MINC_LIB): $(OBJS)
	$(RM) $(RM_FLAGS) $(CC_MINC_LIB)
	$(AR) $(AR_FLAGS) $(CC_MINC_LIB) $(OBJS) $(FORTRAN_OBJ)
	$(RANLIB) $(CC_MINC_LIB)

# Lint everything
lint: $(LINT_MINC_LIB)

$(LINT_MINC_LIB) : $(LINT_LIST)
	$(LINT) -o $(MINC_LIB) $(LINT_LIBOPTS) $(LINTFLAGS) $(LINT_LIST)

# how to make man pages
.man3.3:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files
clean: mostlyclean
	$(RM) $(RM_FLAGS) $(CC_MINC_LIB) $(LINT_MINC_LIB) $(MANPAGES)

mostlyclean:
	$(RM) $(RM_FLAGS) $(OBJS) $(LINT_LIST)

# Copy files to installation directories
install: $(CC_MINC_LIB)
	$(CP) $(CP_FLAGS) $(CC_MINC_LIB) $(INSTALL_LIBDIR)
	$(RANLIB) $(INSTALL_LIBDIR)/lib$(MINC_LIB).a
	for file in $(INSTALLED_HEADERS); do \
      $(CP) $(CP_FLAGS) $$file $(INSTALL_INCDIR)/. ; \
	   $(CHMOD) $(CHMOD_FLAGS) $(INSTALL_INCDIR)/$$file; \
   done
