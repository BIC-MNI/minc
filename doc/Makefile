ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

.SUFFIXES: .tex .dvi

default : build

all : build

build : prog_ref.dvi prog_guide.dvi

.tex.dvi :
	latex $<
	latex $<

clean:
	$(RM) $(RM_FLAGS) *.aux *.dvi *.log *.toc

install:
	@echo Nothing to install
