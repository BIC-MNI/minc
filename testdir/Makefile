# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
PROGS    = minc_test test_mconv minc_test_types test_icv test_icv_range \
	test_icv_dim test_speed test_icv_dim1 test_icv_fillvalue
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
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@


# How to make executables
$(PROGS) : $$@.o  $(CC_MINC_LIB) Makefile
	$(CC) $@.o -o $@ $(CFLAGS) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $$@ln $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_MINC_LIB)

# Run tests
test: $(PROGS)
	run_tests.csh

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(PROG_OBJ)  $(LINT_LIST) $(PROGS)

install:
	@echo Nothing to install
