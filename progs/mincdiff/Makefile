# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mincdiff

# --------------------------------------------------------------------

default: build

all: build

build: $(PROGS)

$(PROGS):
	@echo Cannot create $(PROGS)
	@false

clean:
	@echo $(PROGS) : nothing to remove

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)

