# --------------------------------------------------------------------
#
#     Generic Makefile
#
# if you are creating only one executable, you need only modify the
# following lines:

PROG     = ge_uncompress#                  # executable name
PROG_OBJ = ge_uncompress.o#                # files to compile
OPT      = -O -prototypes#                     # compiler options
LDOPT    = -L/usr/local/lib -lsun -lm -lc_s# ld options
INCLUDES = -I/usr/local/include -I.#           # include directories

# --------------------------------------------------------------------

.SUFFIXES: .ln

CFLAGS    = $(INCLUDES) $(OPT)#           # CFLAGS and LINTFLAGS should
LINTFLAGS = $(INCLUDES)#                  # be same, except for -g/-O

default : $(PROG)

all : $(PROG) lint_$(PROG)

.c.ln:#                                   # defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    # defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

LINT_LIST = $(PROG_OBJ:.o=.ln)#           # list of lint files in program

$(PROG_OBJ) : Makefile

# How to create executable
$(PROG): $(PROG_OBJ)
	$(CC) $(PROG_OBJ) -o $@ $(LDOPT)

# how to lint the executable source
lint_$(PROG): $(LINT_LIST)
	lint -u $(LINTFLAGS) $(LINT_LIST)

clean:
	\rm -f   $(PROG_OBJ)  $(LINT_LIST)
