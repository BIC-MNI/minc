EXTRA_DIST = prog_guide.tex  prog_ref.tex

SUFFIXES = .tex .dvi .ps

.PHONY: doc
doc: prog_ref.ps prog_guide.ps


.tex.dvi:
	latex $<
	latex $<

prog_ref.ps: prog_ref.dvi
	dvips $? -o $@

prog_guide.ps: prog_guide.dvi
	dvips $? -o $@

CLEANFILES = \
	prog_ref.aux prog_ref.dvi prog_ref.log prog_ref.ps prog_ref.toc \
	prog_guide.aux prog_guide.dvi prog_guide.log prog_guide.ps prog_guide.toc
