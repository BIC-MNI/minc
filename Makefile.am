# Makefile.am for MINC2
#
AUTOMAKE_OPTIONS = check-news subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = 
AM_YFLAGS = -d --debug

SUBDIRS = . volume_io testdir doc
if MINC2
SUBDIRS += libsrc2
endif

# Add these to the include searches.
#
INCLUDES = \
	-I$(top_srcdir)/libsrc \
	-I$(top_builddir)/volume_io/Include \
	-I$(top_srcdir)/volume_io/Include \
	-I$(top_srcdir)/progs/Proglib \
	-I$(top_srcdir)/conversion/Acr_nema

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
	progs/Proglib/convert_origin_to_start.h \
	progs/Proglib/minc_def.h \
	progs/minccalc/errx.h \
	progs/minccalc/node.h \
	progs/minccalc/y.tab.h \
	progs/mincresample/mincresample.h \
	progs/mincreshape/mincreshape.h \
	libsrc2/minc2_private.h \
	progs/mincdump/mincdump.h \
	progs/mincdump/dumplib.h \
	progs/mincdump/vardata.h \
	progs/mincgen/generic.h \
	progs/mincgen/genlib.h \
	progs/mincgen/ncgen.h \
	progs/mincgen/ncgentab.h \
	conversion/ecattominc/ecat_file.h \
	conversion/ecattominc/machine_indep.h \
	conversion/ecattominc/ecat_header_definition.h \
	conversion/minctoecat/ecat_write.h \
	conversion/minctoecat/machine_indep.h \
	conversion/nifti1/nifti1.h \
	conversion/nifti1/nifti1_io.h \
	conversion/nifti1/nifti1_local.h \
	conversion/nifti1/znzlib.h \
	conversion/nifti1/analyze75.h \
	conversion/dcm2mnc/acr_element_defs.h \
	conversion/dcm2mnc/dcm2mnc.h \
	conversion/dcm2mnc/dicom_read.h \
	conversion/dcm2mnc/dicom_to_minc.h \
	conversion/dcm2mnc/ext_element_defs.h \
	conversion/dcm2mnc/minc_file.h \
	conversion/dcm2mnc/progress.h \
	conversion/dcm2mnc/siemens_to_dicom.h \
	conversion/dcm2mnc/siemens_header_defs.h \
	conversion/dcm2mnc/siemens_header_table.h \
	conversion/dcm2mnc/spi_element_defs.h \
	conversion/dcm2mnc/string_to_filename.h \
	conversion/dcm2mnc/pms_element_defs.h \
	conversion/dcm2mnc/gems_element_defs.h \
	conversion/vff2mnc/vff2mnc.h

if ACR_NEMA_TOOLS
include_HEADERS += conversion/Acr_nema/acr_nema.h

include_acr_nemadir = $(includedir)/acr_nema

include_acr_nema_HEADERS = \
	conversion/Acr_nema/acr_nema/acr_io.h \
	conversion/Acr_nema/acr_nema/dicom_client_routines.h \
	conversion/Acr_nema/acr_nema/dicom_network.h \
	conversion/Acr_nema/acr_nema/element.h \
	conversion/Acr_nema/acr_nema/file_io.h \
	conversion/Acr_nema/acr_nema/group.h \
	conversion/Acr_nema/acr_nema/message.h \
	conversion/Acr_nema/acr_nema/value_repr.h

else
#
# If not installing the full ACR/NEMA package, just add these headers to 
# list of headers not to be installed.
#
noinst_HEADERS += \
	conversion/Acr_nema/acr_nema.h \
	conversion/Acr_nema/acr_nema/acr_io.h \
	conversion/Acr_nema/acr_nema/dicom_client_routines.h \
	conversion/Acr_nema/acr_nema/dicom_network.h \
	conversion/Acr_nema/acr_nema/element.h \
	conversion/Acr_nema/acr_nema/file_io.h \
	conversion/Acr_nema/acr_nema/group.h \
	conversion/Acr_nema/acr_nema/message.h \
	conversion/Acr_nema/acr_nema/value_repr.h

endif


# Libraries which must be built and installed.
#
lib_LTLIBRARIES = libminc2.la libvolume_io2.la

# Conditionally install the libacr_nema.la library.  It must always be
# built even if it is not installed.
#
if ACR_NEMA_TOOLS
lib_LTLIBRARIES += libacr_nema.la
else
noinst_LTLIBRARIES = libacr_nema.la
endif

# Scripts we install: sh, csh, or perl.
#
dist_bin_SCRIPTS = \
	progs/mincdiff/mincdiff \
	progs/mincedit/mincedit \
	progs/mincheader/mincheader \
	progs/minchistory/minchistory \
	progs/mincpik/mincpik \
	progs/mincview/mincview

# Scripts that we process (to add version #s) before install
bin_SCRIPTS = progs/xfm/xfmflip

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
	mincgen \
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
	xfminvert \
	mincconvert \
	mincdump \
	ecattominc \
	minctoecat \
	mnc2nii \
	nii2mnc \
	upet2mnc \
	dcm2mnc \
	vff2mnc

if ACR_NEMA_TOOLS
bin_PROGRAMS += \
	mincexample1 \
	mincexample2 \
	acr_test \
	dump_acr_nema \
	extract_acr_nema \
	read_acr_nema \
	dicom_test \
	copy_acr_nema \
	byte_swap \
	extract
endif

noinst_PROGRAMS = \
	mincexample1 \
	mincexample2

LDADD = libvolume_io2.la libminc2.la

EXTRA_DIST = GETTING_STARTED \
	$(m4_files) \
	fortran \
	progs/xfm/xfmflip.in \
	libsrc/strdup.c \
	progs/Get_image_offset/get_image_offset.c \
	epm-header.in \
	libsrc/config.h.msvc-win32 \
	Makefile.msvc-win32 \
	conversion/Acr_nema/dicom.txt \
	WHATSNEW-2.0 \
	INSTALL.minc \
	CMakeLists.txt \
	config.h.cmake \
	progs/CMakeLists.txt \
	conversion/CMakeLists.txt \
	cmake-modules/FindBISON.cmake \
	cmake-modules/FindFLEX.cmake \
	cmake-modules/FindHDF5.cmake \
	cmake-modules/FindMINC.cmake \
	cmake-modules/FindNETCDF.cmake

# The fortran directory is not yet automakified, so it is just
# listed in EXTRA_DIST.  This will copy everything including CVS
# files.  We use this hook to get rid of the latter.
dist-hook:
	rm -rf $(distdir)/fortran/CVS $(distdir)/fortran/vms/CVS

m4_files = \
	m4/mni_REQUIRE_LIB.m4 \
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

mincgen_SOURCES = \
	progs/mincgen/escapes.c \
	progs/mincgen/genlib.c \
	progs/mincgen/getfill.c \
	progs/mincgen/init.c \
	progs/mincgen/load.c \
	progs/mincgen/main.c \
	progs/mincgen/ncgentab.y \
	progs/mincgen/ncgenyy.l


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

mincconvert_SOURCES = progs/mincconvert/mincconvert.c

mincdump_SOURCES = \
	progs/mincdump/mincdump.c \
	progs/mincdump/vardata.c \
	progs/mincdump/dumplib.c

ecattominc_SOURCES = \
	conversion/ecattominc/ecattominc.c \
	conversion/ecattominc/insertblood.c \
	conversion/ecattominc/ecat_file.c \
	conversion/ecattominc/machine_indep.c

minctoecat_SOURCES = \
	conversion/minctoecat/minctoecat.c \
	conversion/minctoecat/ecat_write.c \
	conversion/minctoecat/machine_indep.c

nii2mnc_SOURCES = \
	conversion/nifti1/nii2mnc.c \
	conversion/nifti1/nifti1_io.c \
	conversion/nifti1/znzlib.c

mnc2nii_SOURCES = \
	conversion/nifti1/mnc2nii.c \
	conversion/nifti1/nifti1_io.c \
	conversion/nifti1/znzlib.c

upet2mnc_SOURCES = \
	conversion/micropet/upet2mnc.c

dcm2mnc_LDADD = libacr_nema.la $(LDADD)

dcm2mnc_SOURCES = \
	conversion/dcm2mnc/dcm2mnc.c \
	conversion/dcm2mnc/dicom_to_minc.c \
	conversion/dcm2mnc/siemens_to_dicom.c \
	conversion/dcm2mnc/dicom_read.c \
	conversion/dcm2mnc/minc_file.c \
	conversion/dcm2mnc/progress.c \
	conversion/dcm2mnc/string_to_filename.c

vff2mnc_SOURCES = \
	conversion/vff2mnc/vff2mnc.c

acr_test_LDADD = libacr_nema.la

acr_test_SOURCES = conversion/Acr_nema/acr_test.c

dump_acr_nema_LDADD = libacr_nema.la

dump_acr_nema_SOURCES = conversion/Acr_nema/dump_acr_nema.c

extract_acr_nema_LDADD = libacr_nema.la

extract_acr_nema_SOURCES = conversion/Acr_nema/extract_acr_nema.c

read_acr_nema_LDADD = libacr_nema.la

read_acr_nema_SOURCES = conversion/Acr_nema/read_acr_nema.c

dicom_test_LDADD = libacr_nema.la

dicom_test_SOURCES = conversion/Acr_nema/dicom_test.c

copy_acr_nema_LDADD = libacr_nema.la

copy_acr_nema_SOURCES = conversion/Acr_nema/copy_acr_nema.c

extract_SOURCES = conversion/image_filters/extract.c
byte_swap_SOURCES = conversion/image_filters/byte_swap.c


libminc2_la_LDFLAGS = -version-info 2:0:1
libminc2_la_SOURCES = \
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

libvolume_io2_la_LDFLAGS = -version-info 2:0:1
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

libacr_nema_la_SOURCES = \
	conversion/Acr_nema/acr_io.c \
	conversion/Acr_nema/dicom_client_routines.c \
	conversion/Acr_nema/dicom_network.c \
	conversion/Acr_nema/element.c \
	conversion/Acr_nema/file_io.c \
	conversion/Acr_nema/globals.c \
	conversion/Acr_nema/group.c \
	conversion/Acr_nema/message.c \
	conversion/Acr_nema/value_repr.c

# MAN pages go here.  The "dist_" prefix signals that they files
# are to be included in source distribution.
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
	progs/mincgen/mincgen.man1 \
	progs/mincheader/mincheader.man1 \
	progs/minchistory/minchistory.man1 \
	progs/mincinfo/mincinfo.man1 \
	progs/minclookup/minclookup.man1 \
	progs/mincmakescalar/mincmakescalar.man1 \
	progs/mincmakevector/mincmakevector.man1 \
	progs/mincmath/mincmath.man1 \
	progs/mincpik/mincpik.man1 \
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
	progs/xfm/xfminvert.man1 \
	progs/xfm/xfmflip.man1 \
	progs/mincconvert/mincconvert.man1 \
	conversion/ecattominc/ecattominc.man1 \
	conversion/minctoecat/minctoecat.man1 \
	conversion/nifti1/nii2mnc.man1 \
	conversion/nifti1/mnc2nii.man1 \
	conversion/micropet/upet2mnc.man1 \
	conversion/dcm2mnc/dcm2mnc.man1 \
	conversion/vff2mnc/vff2mnc.man1

dist_man3_MANS = libsrc/ParseArgv.man3
