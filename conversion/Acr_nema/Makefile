# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = acr_test dump_acr_nema extract_acr_nema read_acr_nema dicom_test
OBJS     = file_io.o acr_io.o element.o value_repr.o group.o message.o \
	        dicom_network.o
EXTRA_OBJS = globals.o
HEADERS  = $(OBJS:.o=.h) acr_nema.h
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(MACHINE_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_OBJS = $(OBJS:.o=.ln) $(EXTRA_OBJS:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build lint

build: $(CC_ACR_LIB) $(PROGS)

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) $(OBJS) $(EXTRA_OBJS) $(LINT_OBJS) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) $(OBJS) $(EXTRA_OBJS): $(HEADERS)

$(LINT_LIST) $(LINT_OBJS) : $(HEADERS)

#How to create library
$(CC_ACR_LIB): $(OBJS) $(EXTRA_OBJS)
	$(RM) $(RM_FLAGS) $(CC_ACR_LIB)
	$(AR) $(AR_FLAGS) $(CC_ACR_LIB) $(OBJS) $(EXTRA_OBJS)
	$(RANLIB) $(CC_ACR_LIB)

# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(CC_ACR_LIB)
	$(CC) -o $@ $@.o $(CC_ACR_LIB) $(MACHINE_LDOPT)

#  how to lint the executable source
lint: $(LINT_ACR_LIB) $(LINT_LIST_EXE)

# How to create lint library
$(LINT_ACR_LIB) : $(LINT_OBJS)
	$(LINT) -o $(ACR_LIB) $(LINT_LIBOPTS) $(LINTFLAGS) $(LINT_OBJS)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_ACR_LIB)
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_ACR_LIB)

# how to make man pages
.man1.1:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files in this directory
clean: mostlyclean
	$(RM) $(RM_FLAGS) $(CC_ACR_LIB) $(LINT_ACR_LIB)

mostlyclean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ) \
	            $(OBJS) $(EXTRA_OBJS) $(LINT_OBJS) $(MANPAGES)

install:
	@echo Nothing to install.

