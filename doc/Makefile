ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

.SUFFIXES: .tex .dvi

default : build

all : build

build : minc_reference.dvi minc_user.dvi

.tex.dvi :
	latex $<

clean:
	$(RM) $(RM_FLAGS) *.aux *.dvi *.log

install:
	@echo Nothing to install
