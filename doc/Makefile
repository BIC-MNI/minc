ROOT = ..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

DOCS = prog_ref.ps prog_guide.ps

.SUFFIXES : .tex .dvi .ps

default : build

all : build

build : $(DOCS)

$(DOCS) : $(DOCS:.ps=.dvi)

.tex.dvi :
	-latex $<
	-latex $<

.dvi.ps :
	-dvips $< -o $@

clean:
	$(RM) $(RM_FLAGS) *.aux *.dvi *.log *.toc

install:
	@echo Nothing to install
