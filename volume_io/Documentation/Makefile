ROOT = ../..
include $(ROOT)/Make_machine_specific
include $(ROOT)/Make_configuration

DOCS = volume_io.ps

.SUFFIXES : .tex .dvi .ps

default : build

all : build

build : $(DOCS)

.tex.dvi :
	-latex $<
	-latex $<

volume_io.ps: volume_io.dvi
	-dvips $? -o $@

clean: mostlyclean
	$(RM) $(RM_FLAGS) *.aux *.log *.toc

mostlyclean:
	$(RM) $(RM_FLAGS) *.dvi *.ps

install: $(DOCS)
	@if test -z "$(INSTALL_DOCDIR)"; then \
	   echo No documentation installation directory; \
	else \
      for doc in $(DOCS) DUMMY; do \
	      if test -r $$doc ; then \
	         cmd="$(CP) $(CP_FLAGS) $$doc $(INSTALL_DOCDIR)/."; \
	         echo "$$cmd"; \
	         $$cmd; \
	      fi; \
	   done; \
	fi
