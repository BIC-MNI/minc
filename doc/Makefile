ROOT = ..
include $(ROOT)/Makefile_machine_specific
include $(ROOT)/Makefile_configuration

.SUFFIXES: .tex .dvi

default : all

all: minc_reference.dvi minc_user.dvi

.tex.dvi :
	latex $<

clean:
	rm -f *.aux *.dvi *.log
