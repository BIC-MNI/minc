# --------------------------------------------------------------------
#
# MINC Makefile for program library
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

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
   MNI_formats/gen_xfs.o MNI_formats/gen_xf_io.o \
   MNI_formats/thin_plate_spline.o MNI_formats/tag_points.o \
   MNI_formats/mni_io.o \
   Volumes/volumes.o Volumes/input_mnc.o Volumes/input_volume.o \
   Volumes/output_mnc.o Volumes/output_volume.o \
   Volumes/input_free.o

# Header files
HEADERS  = \
   Include/volume_io.h Include/volume_io_prototypes.h \
   Include/basic.h Include/string_funcs.h Include/files.h \
   Include/arrays.h Include/geom_structs.h Include/progress.h \
   Include/geometry.h Include/volume.h Include/transforms.h \
   Include/alloc.h Include/alloc_check.h Include/def_math.h

CDEFINES = #                        cpp defines

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

$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(CC_VOLIO_LIB) $(CC_MINC_LIB)
	$(CC) $@.o -o $@ $(LDOPT)

# Lint
lint : $(LINT_VOLIO_LIB) $(LINT_LIST_EXE)

# How to create lint library
$(LINT_VOLIO_LIB) : $(LINT_OBJS)
	lint -o $(VOLIO_LIB) -x -u $(LINTFLAGS) $(LINT_OBJS)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_VOLIO_LIB) $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_VOLIO_LIB) $(LINT_MINC_LIB)

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(CC_VOLIO_LIB) $(LINT_VOLIO_LIB) \
	         $(OBJS) $(LINT_OBJS) $(PROGS) $(PROG_OBJ) $(LINT_LIST)

install:
	$(CP) $(CP_FLAGS) $(CC_VOLIO_LIB) $(INSTALL_LIBDIR)
	$(CP) $(CP_FLAGS) $(HEADERS) $(INSTALL_VOLIO_INCDIR)
	$(CHMOD) $(CHMOD_FLAGS) \
	      `echo $(HEADERS) | sed 's+Include/+$(INSTALL_VOLIO_INCDIR)/+g'`
