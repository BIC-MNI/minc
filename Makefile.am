# Makefile.am for MINC
#
AUTOMAKE_OPTIONS = check-news subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = . volume_io testdir doc


# Add these to the include searches.
#
INCLUDES = \
	-I$(top_srcdir)/libsrc \
	-I$(top_builddir)/volume_io/Include \
	-I$(top_srcdir)/volume_io/Include \
	-I$(top_srcdir)/progs/Proglib

# Headers which must be installed.
#
include_HEADERS = \
	libsrc/minc.h \
	libsrc/ParseArgv.h \
	libsrc/voxel_loop.h \
	libsrc/nd_loop.h \
	libsrc/time_stamp.h \
	volume_io/Include/volume_io.h

include_volume_iodir = $(includedir)/volume_io

include_volume_io_HEADERS = \
	volume_io/Include/alloc.h \
	volume_io/Include/arrays.h \
	volume_io/Include/basic.h \
	volume_io/Include/def_math.h \
	volume_io/Include/files.h \
	volume_io/Include/geom_structs.h \
	volume_io/Include/geometry.h \
	volume_io/Include/internal_volume_io.h \
	volume_io/Include/multidim.h \
	volume_io/Include/progress.h \
	volume_io/Include/string_funcs.h \
	volume_io/Include/system_dependent.h \
	volume_io/Include/transforms.h \
	volume_io/Include/vol_io_prototypes.h \
	volume_io/Include/volume.h \
	volume_io/Include/volume_cache.h

# Header files which must be included in the distribution but which are 
# not part of the installation.
#
noinst_HEADERS = \
	libsrc/minc_basic.h \
	libsrc/minc_private.h \
	libsrc/minc_routines.h \
	libsrc/minc_structures.h \
	libsrc/minc_useful.h \
	libsrc/minc_varlists.h \
	libsrc/type_limits.h \
	progs/Proglib/convert_origin_to_start.h \
	progs/Proglib/minc_def.h \
	progs/minccalc/errx.h \
	progs/minccalc/node.h \
	progs/minccalc/y.tab.h \
	progs/mincresample/mincresample.h \
	progs/mincreshape/mincreshape.h

# Libraries which must be built and installed.
#
lib_LTLIBRARIES = libminc.la libvolume_io.la

# Scripts we install.  These depend on /bin/sh or /bin/csh
#
dist_bin_SCRIPTS = \
	progs/mincdiff/mincdiff \
	progs/mincedit/mincedit \
	progs/mincheader/mincheader \
	progs/mincview/mincview


# Programs we build and install.
#
bin_PROGRAMS = \
	voxeltoworld \
	worldtovoxel \
	minc_modify_header \
	mincaverage \
	minccalc \
	mincconcat \
	minccopy \
	mincexpand \
	mincextract \
	mincinfo \
	minclookup \
	mincmakescalar \
	mincmakevector \
	mincmath \
	mincresample \
	mincreshape \
	mincstats \
	minctoraw \
	invert_raw_image \
	mincwindow \
	rawtominc \
	transformtags \
	xfmconcat \
	xfminvert

noinst_PROGRAMS = \
	mincexample1 \
	mincexample2

LDADD = libvolume_io.la libminc.la

EXTRA_DIST = GETTING_STARTED $(m4_files) \
	fortran \
	libsrc/strdup.c \
	progs/Get_image_offset/get_image_offset.c \
	mni_minc.epm.header

# The fortran directory is not yet automakified, so it is just
# listed in EXTRA_DIST.  This will copy everything including CVS
# files.  We use this hook to get rid of the latter.
dist-hook:
	rm -rf $(distdir)/fortran/CVS $(distdir)/fortran/vms/CVS

m4_files = \
	m4/mni_REQUIRE_LIB.m4 \
	m4/mni_REQUIRE_MNILIBS.m4 \
	m4/mni_REQUIRE_OPENINVENTOR.m4 \
	m4/mni_cxx_have_koenig_lookup.m4 \
	m4/smr_CGAL_MAKEFILE.m4 \
	m4/smr_OPTIONAL_LIB.m4 \
	m4/smr_REQUIRED_LIB.m4 \
	m4/smr_WITH_BUILD_PATH.m4


voxeltoworld_SOURCES = progs/coordinates/voxeltoworld.c

worldtovoxel_SOURCES = progs/coordinates/worldtovoxel.c

minc_modify_header_SOURCES = progs/minc_modify_header/minc_modify_header.c

mincaverage_SOURCES = progs/mincaverage/mincaverage.c

minccalc_CFLAGS = -Iprogs/minccalc -I$(srcdir)/progs/minccalc
minccalc_SOURCES = \
	progs/minccalc/minccalc.c \
	progs/minccalc/gram.y \
	progs/minccalc/eval.c \
	progs/minccalc/ident.c \
	progs/minccalc/lex.l \
	progs/minccalc/node.c \
	progs/minccalc/optim.c \
	progs/minccalc/scalar.c \
	progs/minccalc/sym.c \
	progs/minccalc/vector.c

mincconcat_SOURCES = progs/mincconcat/mincconcat.c

minccopy_SOURCES = progs/minccopy/minccopy.c

mincexample1_SOURCES = progs/mincexample/mincexample1.c

mincexample2_SOURCES = progs/mincexample/mincexample2.c

mincexpand_SOURCES = progs/mincexpand/mincexpand.c

mincextract_SOURCES = progs/mincextract/mincextract.c

mincinfo_SOURCES = progs/mincinfo/mincinfo.c

minclookup_SOURCES = progs/minclookup/minclookup.c

mincmakescalar_SOURCES = progs/mincmakescalar/mincmakescalar.c

mincmakevector_SOURCES = progs/mincmakevector/mincmakevector.c

mincmath_SOURCES = progs/mincmath/mincmath.c

mincresample_SOURCES = \
	progs/mincresample/mincresample.c \
	progs/mincresample/resample_volumes.c \
	progs/Proglib/convert_origin_to_start.c

mincreshape_SOURCES = \
	progs/mincreshape/mincreshape.c \
	progs/mincreshape/copy_data.c

mincstats_SOURCES = progs/mincstats/mincstats.c

minctoraw_SOURCES = progs/minctoraw/minctoraw.c

invert_raw_image_SOURCES = progs/mincview/invert_raw_image.c

mincwindow_SOURCES = progs/mincwindow/mincwindow.c

rawtominc_SOURCES = \
	progs/rawtominc/rawtominc.c \
	progs/Proglib/convert_origin_to_start.c

transformtags_SOURCES = progs/xfm/transformtags.c

xfmconcat_SOURCES = progs/xfm/xfmconcat.c

xfminvert_SOURCES = progs/xfm/xfminvert.c

libminc_la_LDFLAGS = -version-info 0:0:0
libminc_la_SOURCES = \
	libsrc/ParseArgv.c \
	libsrc/dim_conversion.c \
	libsrc/image_conversion.c \
	libsrc/minc_convenience.c \
	libsrc/minc_error.c \
	libsrc/minc_globdef.c \
	libsrc/nd_loop.c \
	libsrc/netcdf_convenience.c \
	libsrc/time_stamp.c \
	libsrc/value_conversion.c \
	libsrc/voxel_loop.c

libvolume_io_la_LDFLAGS = -version-info 0:0:0
libvolume_io_la_SOURCES = \
	volume_io/Geometry/colour.c \
	volume_io/Geometry/colour_def.c \
	volume_io/Geometry/gaussian.c \
	volume_io/Geometry/inverse.c \
	volume_io/Geometry/newton.c \
	volume_io/Geometry/points.c \
	volume_io/Geometry/splines.c \
	volume_io/Geometry/tensors.c \
	volume_io/Geometry/transforms.c \
	volume_io/MNI_formats/gen_xf_io.c \
	volume_io/MNI_formats/gen_xfs.c \
	volume_io/MNI_formats/grid_transforms.c \
	volume_io/MNI_formats/mni_io.c \
	volume_io/MNI_formats/tag_points.c \
	volume_io/MNI_formats/thin_plate_spline.c \
	volume_io/Prog_utils/alloc.c \
	volume_io/Prog_utils/alloc_check.c \
	volume_io/Prog_utils/arrays.c \
	volume_io/Prog_utils/files.c \
	volume_io/Prog_utils/print.c \
	volume_io/Prog_utils/progress.c \
	volume_io/Prog_utils/string.c \
	volume_io/Prog_utils/time.c \
	volume_io/Volumes/evaluate.c \
	volume_io/Volumes/get_hyperslab.c \
	volume_io/Volumes/input_free.c \
	volume_io/Volumes/input_mnc.c \
	volume_io/Volumes/input_volume.c \
	volume_io/Volumes/multidim_arrays.c \
	volume_io/Volumes/output_mnc.c \
	volume_io/Volumes/output_volume.c \
	volume_io/Volumes/set_hyperslab.c \
	volume_io/Volumes/volume_cache.c \
	volume_io/Volumes/volumes.c

# MAN pages go here.  
# Using "dist_" should force them to be added the distribution.
#
dist_man1_MANS = \
	progs/coordinates/voxeltoworld.man1 \
	progs/minc_modify_header/minc_modify_header.man1 \
	progs/mincaverage/mincaverage.man1 \
	progs/minccalc/minccalc.man1 \
	progs/mincconcat/mincconcat.man1 \
	progs/minccopy/minccopy.man1 \
	progs/mincdiff/mincdiff.man1 \
	progs/mincedit/mincedit.man1 \
	progs/mincexpand/mincexpand.man1 \
	progs/mincextract/mincextract.man1 \
	progs/mincheader/mincheader.man1 \
	progs/mincinfo/mincinfo.man1 \
	progs/minclookup/minclookup.man1 \
	progs/mincmakescalar/mincmakescalar.man1 \
	progs/mincmakevector/mincmakevector.man1 \
	progs/mincmath/mincmath.man1 \
	progs/mincresample/mincresample.man1 \
	progs/mincreshape/mincreshape.man1 \
	progs/mincstats/mincstats.man1 \
	progs/minctoraw/minctoraw.man1 \
	progs/mincview/invert_raw_image.man1 \
	progs/mincview/mincview.man1 \
	progs/mincwindow/mincwindow.man1 \
	progs/rawtominc/rawtominc.man1 \
	progs/xfm/transformtags.man1 \
	progs/xfm/xfmconcat.man1 \
	progs/xfm/xfminvert.man1

dist_man3_MANS = libsrc/ParseArgv.man3
