# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
PROGS    = mincheader
MANSECT  = 1
MANPAGES = $(PROGS).$(MANSECT)

# --------------------------------------------------------------------

#Suffixes for man pages
.SUFFIXES: .1 .man1 .3 .man3

default: build

all: build

build: man

man: $(MANPAGES)

# how to make man pages
.man1.1:
	$(NROFF) $(NROFF_FLAGS) $< > $@

clean:
	$(RM) $(RM_FLAGS) $(MANPAGES)

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)
	$(CP) $(CP_FLAGS) $(MANPAGES) $(INSTALL_MANDIR)$(MANSECT)

