# Makefile.am for MINC2
#
AUTOMAKE_OPTIONS = check-news subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = 
AM_YFLAGS = -d --debug

SUBDIRS = . volume_io testdir doc
if MINC2
SUBDIRS += libsrc2/test
endif

# Add these to the include searches.
#
INCLUDES = \
	-I$(top_srcdir)/libsrc \
	-I$(top_builddir)/volume_io/Include \
	-I$(top_srcdir)/volume_io/Include \

if MINC2
INCLUDES += -I$(top_srcdir)/libsrc2
endif

# Headers which must be installed.
#
include_HEADERS = \
	libsrc/minc.h \
	libsrc/ParseArgv.h \
	libsrc/voxel_loop.h \
	libsrc/nd_loop.h \
	libsrc/time_stamp.h \
	libsrc/minc_compat.h \
	volume_io/Include/volume_io.h \
	libsrc2/minc2.h \
	libsrc/minc_simple.h

include_volume_iodir = $(includedir)/volume_io

include_volume_io_HEADERS = \
	volume_io/Include/volume_io/alloc.h \
	volume_io/Include/volume_io/arrays.h \
	volume_io/Include/volume_io/basic.h \
	volume_io/Include/volume_io/def_math.h \
	volume_io/Include/volume_io/files.h \
	volume_io/Include/volume_io/geom_structs.h \
	volume_io/Include/volume_io/geometry.h \
	volume_io/Include/internal_volume_io.h \
	volume_io/Include/volume_io/multidim.h \
	volume_io/Include/volume_io/progress.h \
	volume_io/Include/volume_io/string_funcs.h \
	volume_io/Include/volume_io/system_dependent.h \
	volume_io/Include/volume_io/transforms.h \
	volume_io/Include/volume_io/vol_io_prototypes.h \
	volume_io/Include/volume_io/volume.h \
	volume_io/Include/volume_io/volume_cache.h

# Header files which must be included in the distribution but which are 
# not part of the installation.
#
noinst_HEADERS = \
	libsrc/read_file_names.h \
	libsrc/minc_basic.h \
	libsrc/minc_config.h \
	libsrc/minc_error.h \
	libsrc/minc_private.h \
	libsrc/minc_routines.h \
	libsrc/minc_structures.h \
	libsrc/minc_useful.h \
	libsrc/minc_varlists.h \
	libsrc/type_limits.h \
	libsrc/hdf_convenience.h \
	libsrc2/minc2_private.h 

# Libraries which must be built and installed.
#
lib_LTLIBRARIES = libminc2.la libvolume_io2.la


noinst_PROGRAMS = \
	mincexample1 \
	mincexample2

LDADD = libvolume_io2.la libminc2.la

EXTRA_DIST = \
	$(m4_files) \
	$(ezMINC_files) \
	libsrc/strdup.c \
	epm-header.in \
	libsrc/config.h.msvc-win32 \
	Makefile.msvc-win32 \
	conversion/Acr_nema/dicom.txt \
	INSTALL.minc \
	CMakeLists.txt \
	config.h.cmake \
	UseLIBMINC.cmake.in \
	LIBMIN2Config.cmake.in \
	conversion/CMakeLists.txt \
	cmake-modules/FindHDF5.cmake \
	cmake-modules/FindNETCDF.cmake

m4_files = \
	m4/mni_REQUIRE_LIB.m4 \
	m4/smr_WITH_BUILD_PATH.m4

ezMINC_files = \
	ezminc/examples/volume_msq_dist.cpp \
	ezminc/examples/CMakeLists.txt \
	ezminc/examples/fuzzy_volume_similarity.cpp \
	ezminc/examples/trilinear_resample.cpp \
	ezminc/examples/volume_avg.cpp \
	ezminc/examples/volume_gtc_similarity.cpp \
	ezminc/examples/volume_similarity.cpp \
	ezminc/minc_1_simple_rw.cpp \
	ezminc/minc_1_simple_rw.h \
	ezminc/minc_io_4d_volume.h \
	ezminc/minc_io_fixed_vector.h \
	ezminc/minc_io_simple_volume.h \
	ezminc/tests/minc_rw_test2.cpp \
	ezminc/tests/minc_rw_test_4d.cpp \
	ezminc/tests/minc_rw_test_simple.cpp \
	ezminc/tests/CMakeLists.txt \
	ezminc/tests/ezminc_rw_test.cpp \
	ezminc/tests/ezminc_stats.cpp \
	ezminc/minc_1_simple.h \
	ezminc/CMakeLists.txt \
	ezminc/minc_1_rw.cpp \
	ezminc/minc_1_rw.h \
	ezminc/minc_io_exceptions.h \
	
libminc2_la_LDFLAGS = -version-info 3:0:0
libminc2_la_SOURCES = \
	libsrc/ParseArgv.c \
	libsrc/read_file_names.c \
	libsrc/dim_conversion.c \
	libsrc/image_conversion.c \
	libsrc/minc_convenience.c \
	libsrc/minc_error.c \
	libsrc/nd_loop.c \
	libsrc/netcdf_convenience.c \
	libsrc/time_stamp.c \
	libsrc/value_conversion.c \
	libsrc/voxel_loop.c \
	libsrc/hdf_convenience.c \
	libsrc/minc_compat.c \
	libsrc/minc_simple.c

if MINC2
libminc2_la_SOURCES += \
	libsrc2/convert.c \
	libsrc2/datatype.c \
	libsrc2/dimension.c \
	libsrc2/free.c \
	libsrc2/grpattr.c \
	libsrc2/hyper.c \
	libsrc2/label.c \
	libsrc2/m2util.c \
	libsrc2/record.c \
	libsrc2/slice.c \
	libsrc2/valid.c \
	libsrc2/volprops.c \
	libsrc2/volume.c
else
EXTRA_DIST += \
	libsrc2/convert.c \
	libsrc2/datatype.c \
	libsrc2/dimension.c \
	libsrc2/free.c \
	libsrc2/grpattr.c \
	libsrc2/hyper.c \
	libsrc2/label.c \
	libsrc2/m2util.c \
	libsrc2/record.c \
	libsrc2/slice.c \
	libsrc2/valid.c \
	libsrc2/volprops.c \
	libsrc2/volume.c
endif

libvolume_io2_la_LDFLAGS = -version-info 3:0:0
libvolume_io2_la_SOURCES = \
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

dist_man3_MANS = libsrc/ParseArgv.man3
