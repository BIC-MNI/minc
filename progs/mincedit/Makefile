# --------------------------------------------------------------------
#
# MINC Makefile
#

ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

# Executable names
PROGS    = mincedit

# --------------------------------------------------------------------

default: build

all: build

build: $(PROGS)

$(PROGS):
	@echo Cannot create $(PROGS)
	@false

mostlyclean clean:
	@echo $(PROGS) : nothing to remove

install:
	$(CP) $(CP_FLAGS) $(PROGS) $(INSTALL_BINDIR)
	$(CD) $(INSTALL_BINDIR); $(CHMOD) $(CHMOD_FLAGS) $(PROGS)

