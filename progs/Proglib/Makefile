# --------------------------------------------------------------------
#
# MINC Makefile for program library
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Object names
OBJS     = ParseArgv.o vax_conversions.o time_stamp.o \
	   nrutil.o ludcmp.o lubksb.o \
	   gen_transforms.o gen_transform_io.o thin_plate_spline.o \
      tag_points.o mni_io.o\
	   transforms.o inverse.o \
	   alloc.o alloc_check.o print.o numerical.o files.o random.o time.o \
      string.o 
CDEFINES = #                        cpp defines

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

LINT_OBJS = $(OBJS:.o=.ln)

# --------------------------------------------------------------------

default: build

all: build lint

build: $(CC_PROG_LIB)

#Dependency on Makefile
$(OBJS) $(LINT_OBJS) : Makefile

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
	$(RM) $(RM_FLAGS) $(CC_PROG_LIB) $(LINT_PROG_LIB) $(OBJS) $(LINT_OBJS)

