# --------------------------------------------------------------------
#
#     Generic Makefile
#
PROG     = ge_uncompress	# executable name
PROG_OBJ = ge_uncompress.o	# files to compile
OPT      = -O			# compiler options
LDOPT    = -L/usr/local/lib	# ld options
INCLUDES = -I/usr/local/include -I. # include directories

PERL_PROGS = ge4_to_minc ge5_to_minc siemens_to_minc dicom_to_minc \
   siemens_magnetom_vision_to_minc gedicom_to_minc dicomfile_to_minc
PERL_INCLUDE = mri_to_minc.pl

# --------------------------------------------------------------------

.SUFFIXES: .ln .pl

CFLAGS    = $(INCLUDES) $(OPT)	# CFLAGS and LINTFLAGS should
LINTFLAGS = $(INCLUDES)		# be same, except for -g/-O

default : $(PROG) $(PERL_PROGS)

all : $(PROG) lint_$(PROG) $(PERL_PROGS)

.c.ln:				# defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:				# defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

.pl:
	cat $(PERL_INCLUDE) $@.pl > $@; chmod +x $@

LINT_LIST = $(PROG_OBJ:.o=.ln)	# list of lint files in program

$(PROG_OBJ) : Makefile

# How to create executable
$(PROG): $(PROG_OBJ)
	$(CC) $(PROG_OBJ) -o $@ $(LDOPT)

# how to lint the executable source
lint_$(PROG): $(LINT_LIST)
	lint -u $(LINTFLAGS) $(LINT_LIST)

# How to create perl programs from multiple source files
$(PERL_PROGS): $(PERL_INCLUDE) Makefile

clean:
	\rm -f $(PROG)  $(PROG_OBJ)  $(LINT_LIST) $(PERL_PROGS)
