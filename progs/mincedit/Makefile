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

build: $(PROGS)

$(PROGS):
	@echo Cannot create $(PROGS)
	@false

clean:
	@echo $(PROGS) : nothing to remove

install: build
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)

