# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

# Executable names
PROGS    = mincedit

# --------------------------------------------------------------------

default: build

all: build

build:
	@echo $(PROGS) is up to date

clean:
	@echo $(PROGS) : nothing to remove

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)

