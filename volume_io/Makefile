# --------------------------------------------------------------------
#
# MINC Makefile for program library
#

ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Compiler options
LDOPT    = $(CC_VOLIO_LIB) -lm $(MINC_LDOPT)
CDEFINES = $(GLOBAL_CDEFINES)

# Source definitions 
include ProgFiles
include SourceFiles
include HeaderFiles
include DocFiles

# Object names
PROGS = $(PROGSRCS:.c=)
OBJS = $(SRCS:.c=.o)

# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

LINT_OBJS = $(OBJS:.o=.ln)

PROG_OBJ  = $(PROGS:=.o)
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)

# --------------------------------------------------------------------

.SUFFIXES: .tex .dvi .ps

default: build

all: build lint

build: $(CC_VOLIO_LIB) $(PROGS)

#Dependency on Makefile
$(OBJS) $(LINT_OBJS) $(PROG_OBJ) $(LINT_LIST) : Makefile

# Make a symlink so that we can include <volume_io/foo.h>
$(OBJS): Include/volume_io
Include/volume_io:
	ln -s . Include/volume_io

.c.ln:#                                   defines the rule for creating .ln
	$(LINT) $(LINTFLAGS) -c $<
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
	$(LINT) -o $(VOLIO_LIB) $(LINT_LIBOPTS) $(LINTFLAGS) $(LINT_OBJS)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_VOLIO_LIB) $(LINT_MINC_LIB)
	$(LINT) $(LINT_PROGOPTS) $(LINTFLAGS) $@ln $(LINT_VOLIO_LIB) \
	      $(LINT_MINC_LIB)

# Remove all derived files in this directory
clean: mostlyclean
	$(RM) $(RM_FLAGS) $(CC_VOLIO_LIB) $(LINT_VOLIO_LIB)

mostlyclean:
	$(RM) $(RM_FLAGS) $(OBJS) $(LINT_OBJS) $(PROGS) $(PROG_OBJ) $(LINT_LIST)

install: $(CC_VOLIO_LIB)
	$(CP) $(CP_FLAGS) $(CC_VOLIO_LIB) $(INSTALL_LIBDIR)
	$(RANLIB) $(INSTALL_LIBDIR)/lib$(VOLIO_LIB).a
	$(CP) $(CP_FLAGS) Include/volume_io.h $(INSTALL_INCDIR)
	$(CP) $(CP_FLAGS) $(VOLIO_HEADERS) $(INSTALL_VOLIO_INCDIR)
	$(CHMOD) $(CHMOD_FLAGS) \
	      `echo $(VOLIO_HEADERS) | sed 's+Include/+$(INSTALL_VOLIO_INCDIR)/+g'`
