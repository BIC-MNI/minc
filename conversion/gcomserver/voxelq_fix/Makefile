# --------------------------------------------------------------------
# 
# Generic Makefile 
# 
# if you are creating only one executable, you need only modify the 
# following lines:

PROG     = test_vq_fix #                     - program name.
PROG_OBJ = test_vq_fix.o voxelq_fix_coords.o # - program objects (and subroutines).
OPT      = -O #                           - compiler options.
INCLUDES = -I../../Acr_nema #             - include paths.
LDFLAGS  = ../../Acr_nema/libacrnema.a -lm # - load flags.

# --------------------------------------------------------------------

.SUFFIXES: .ln                            # tell make to watch for .ln's

CFLAGS    = $(OPT) $(INCLUDES)            # CFLAGS and LINTFLAGS should
LINTFLAGS = -m -x -u $(INCLUDES)          # be same, except for -g/-O

.c.ln:                        # defines the rule for creating .ln, may be -i for some platforms
	lint $(LINTFLAGS) -c $< -o $@    

.c.o:                         # defines the rule for creating .o,this is automatically defined)
	$(CC) $(CFLAGS) -c $< -o $@  

LINT_LIST = $(PROG_OBJ:.o=.ln)            # list of lint files in program

build: $(PROG)

lint: lint_$(PROG)

all: build lint

$(PROG): $(PROG_OBJ)                      # how to create executable
	$(CC) $(PROG_OBJ) -o $@ $(LDFLAGS)

lint_$(PROG): $(LINT_LIST)                # how to lint the executable source
	lint -u $(LINTFLAGS) $(LINT_LIST)

clean:
	\rm -f   $(PROG_OBJ)  $(LINT_LIST)
