# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = minc test_mconv minc_types icv icv_range \
	icv_dim test_speed icv_dim1 icv_fillvalue
CDEFINES = -DDEBUG#                        cpp defines
LDOPT    = $(MINC_LDOPT)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)#           list of lint files in program
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint

# --------------------------------------------------------------------

default: build

all: build test

build: $(PROGS)

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@


# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o  $(CC_MINC_LIB) Makefile
	$(CC) $@.o -o $@ $(CFLAGS) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_MINC_LIB)
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_MINC_LIB)

# Run tests
test: $(PROGS)
	./run_tests.csh

# Remove all derived files in this directory
mostlyclean clean:
	$(RM) $(RM_FLAGS) $(PROG_OBJ)  $(LINT_LIST) $(PROGS)

install:
	@echo Nothing to install
