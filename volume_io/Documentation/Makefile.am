EXTRA_DIST = volume_io.tex

SUFFIXES = .tex .dvi .ps

.PHONY: docs
docs: volume_io.ps


.tex.dvi:
	latex $<
	latex $<

volume_io.ps: volume_io.dvi
	dvips $? -o $@

CLEANFILES = \
	volume_io.aux volume_io.dvi volume_io.log volume_io.ps volume_io.toc
