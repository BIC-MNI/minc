# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = gcomserver gcomserver-debug gyrotominc
EXTRA_OBJS = spi_element_defs.o open_connection.o reply.o spi_message.o \
             save_transferred_object.o use_the_files.o \
             gyro_to_minc.o gyro_read.o minc_file.o \
             string_to_filename.o
HEADERS  = gcomserver.h gcom_prototypes.h spi_element_defs.h gyro_to_minc.h
MANSECT  = 1
CDEFINES = -DDEBUG#                        cpp defines
INCLUDES = -I. -I$(ACR_LIB_DIR) -I$(PROG_LIB_DIR) -I$(VOLIO_LIB_DIR)/Include \
	            -I$(MINC_LIB_DIR) -I$(NETCDF_INCLUDE_DIR)
LDOPT    = -lm $(CC_ACR_LIB) $(CC_PROG_LIB) $(CC_VOLIO_LIB) $(MINC_LDOPT)
# --------------------------------------------------------------------

CFLAGS    = $(CDEFINES) $(INCLUDES) $(OPT)# CFLAGS and LINTFLAGS should
LINTFLAGS = $(CDEFINES) $(INCLUDES)#        be same, except for -g/-O

PROG_OBJ  = $(PROGS:=.o)#                 list of objects
LINT_LIST = $(PROG_OBJ:.o=.ln)
LINT_EXTRA= $(EXTRA_OBJS:.o=.ln)
LINT_LIST_EXE = $(LINT_LIST:.ln=.)#       list of executable names to lint
MANPAGES = $(PROGS).$(MANSECT)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build lint

build: $(PROGS)

man: $(MANPAGES)

prototypes:
	extract_functions -public $(PROGS:=.c) $(EXTRA_OBJS:.o=.c) \
	         | perl -ne 's/\)\s+$$/\);\n/; print;' \
	            > gcom_prototypes.h

#Dependency on Makefile
$(PROG_OBJ) $(LINT_LIST) $(EXTRA_OBJS) $(LINT_EXTRA) : Makefile

.c.ln:#                                   defines the rule for creating .ln
	lint $(LINTFLAGS) -c $< -o $@

.c.o:#                                    defines the rule for creating .o
	$(CC) $(CFLAGS) -c $< -o $@

#Dependency of .o and .ln on .h
$(PROG_OBJ) $(EXTRA_OBJS) : $(HEADERS)

$(LINT_LIST) $(LINT_EXTRA) : $(HEADERS)

# Dependency of debug object file on gcomserver.c
gcomserver-debug.o gcomserver-debug.ln : gcomserver.c

# How to make executables
$(PROGS) : $(TARGET_IN_DEPENDENCY).o $(EXTRA_OBJS) $(CC_ACR_LIB) $(CC_PROG_LIB) $(CC_MINC_LIB)
	$(CC) -o $@ $@.o $(EXTRA_OBJS) $(LDOPT)

#  how to lint the executable source
lint: $(LINT_LIST_EXE)

$(LINT_LIST_EXE) : $(TARGET_IN_DEPENDENCY)ln $(LINT_EXTRA) $(LINT_ACR_LIB) $(LINT_PROG_LIB) $(LINT_MINC_LIB)
	lint -u $(LINTFLAGS) $@ln $(LINT_EXTRA) $(LINT_ACR_LIB) $(LINT_PROG_LIB) $(LINT_MINC_LIB)

# how to make man pages
.man1.1:
	$(NROFF) $(NROFF_FLAGS) $< > $@

# Remove all derived files in this directory
clean:
	$(RM) $(RM_FLAGS) $(LINT_LIST) $(PROGS) $(PROG_OBJ) \
	            $(EXTRA_OBJS) $(LINT_EXTRA)
# $(MANPAGES)

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)
#	$(CP) $(CP_FLAGS) $(MANPAGES) $(INSTALL_MANDIR)$(MANSECT)

