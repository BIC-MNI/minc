# --------------------------------------------------------------------
#
# MINC Makefile for program library
#

ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Example program
PROGS = example/convert_volume_to_byte
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(CC_VOLIO_LIB) -lm $(MINC_LDOPT)

# Object names
OBJS     = \
   Prog_utils/alloc.o Prog_utils/alloc_check.o Prog_utils/print.o \
   Prog_utils/files.o Prog_utils/time.o Prog_utils/string.o \
   Prog_utils/progress.o \
   Geometry/transforms.o Geometry/inverse.o Geometry/points.o \
   Geometry/lubksb.o Geometry/ludcmp.o \
   MNI_formats/gen_transforms.o MNI_formats/gen_transform_io.o \
   MNI_formats/thin_plate_spline.o MNI_formats/tag_points.o \
   MNI_formats/mni_io.o \
   Volumes/volumes.o Volumes/input_mnc.o Volumes/input_volume.o \
   Volumes/output_mnc.o Volumes/output_volume.o \
   Volumes/input_free.o

CDEFINES = #                        cpp defines

INCLUDES = -IInclude -I. -I$(PROG_LIB_DIR) -I$(PROG_LIB_DIR)/dave_includes \
	      -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

LINT_OBJS = $(OBJS:.o=.ln)

PROG_OBJ  = $(PROGS:=.o)
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)

# --------------------------------------------------------------------

default: build

all: build lint

build: $(CC_VOLIO_LIB) $(PROGS)

#Dependency on Makefile
$(OBJS) $(LINT_OBJS) $(PROG_OBJ) $(LINT_LIST) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $<
	@if [ "`echo $@ | sed -e 's/.*\///'`" != "$@" ]; \
	then \
	   mv `echo $@ | sed -e 's/.*\///'` $@; \
	fi

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

# How to create library
$(CC_VOLIO_LIB): $(OBJS)
	$(RM) $(RM_FLAGS) $(CC_VOLIO_LIB)
	$(AR) $(AR_FLAGS) $(CC_VOLIO_LIB) $(OBJS)
	$(RANLIB) $(CC_VOLIO_LIB)

$(PROGS) : $$@.o $(CC_VOLIO_LIB) $(CC_MINC_LIB)
	$(CC) $@.o -o $@ $(LDOPT)

# Lint
lint : $(LINT_VOLIO_LIB) $(LINT_LIST_EXE)

# How to create lint library
$(LINT_VOLIO_LIB) : $(LINT_OBJS)
	lint -o $(VOLIO_LIB) -x -u $(LINTFLAGS) $(LINT_OBJS)

$(LINT_LIST_EXE) : $$@ln $(LINT_VOLIO_LIB) $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_VOLIO_LIB) $(LINT_MINC_LIB)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(CC_VOLIO_LIB) $(LINT_VOLIO_LIB) \
	         $(OBJS) $(LINT_OBJS) $(PROGS) $(PROG_OBJ) $(LINT_LIST)

