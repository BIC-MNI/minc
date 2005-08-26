# Programming examples and test programs.

INCLUDES = \
        -I$(top_srcdir)/libsrc \
        -I$(top_builddir)/volume_io/Include \
        -I$(top_srcdir)/volume_io/Include \
        -I$(top_srcdir)/progs/Proglib

LDADD = ../../libvolume_io2.la ../../libminc2.la

noinst_PROGRAMS = example_modify example_tags example_volume_io
check_PROGRAMS = test-xfm

TESTS = check_xfm.sh

EXTRA_DIST = $(TESTS) \
	t1.xfm t2.xfm t3.xfm t3_grid_0.mnc

