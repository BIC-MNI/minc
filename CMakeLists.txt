
PROJECT(libminc2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

#SET(CMAKE_VERBOSE_MAKEFILE ON)

#ADD_SUBDIRECTORY( testdir )
#ADD_SUBDIRECTORY( doc )


# Packaging defines
SET(CPACK_GENERATOR TGZ)
SET(CPACK_PACKAGE_VERSION_MAJOR 2)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 14)
include(CPack)

SET(MINC2 "1")
SET(PACKAGE "minc")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")
SET(PACKAGE_NAME "minc")
SET(PACKAGE_VERSION "2.0.14")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


# check for prereqs
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)

#INCLUDE(CheckIncludeFiles)
# CHECK_INCLUDE_FILES(tmpnam.h

# netcdf
SET(NETCDF_FIND_REQUIRED TRUE)
SET(HDF5_FIND_REQUIRED TRUE)
FIND_PACKAGE(HDF5)
FIND_PACKAGE(NETCDF)

INCLUDE_DIRECTORIES( ${NETCDF_INCLUDE_DIR} ${HDF5_INCLUDE_DIR} )

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_SOURCE_DIR}/config.h)

# LINK_LIBRARIES( ${NETCDF_LIBRARY} ${HDF5_LIBRARY} )

# set the master INCLUDE directories
INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_SOURCE_DIR}/libsrc
   ${CMAKE_SOURCE_DIR}/libsrc2
   ${CMAKE_SOURCE_DIR}/volume_io/Include
   )

# some variables
SET(minc1_LIB_SRCS
   libsrc/ParseArgv.c
   libsrc/dim_conversion.c
   libsrc/image_conversion.c
   libsrc/minc_convenience.c
   libsrc/minc_error.c
   libsrc/minc_globdef.c
   libsrc/nd_loop.c
   libsrc/netcdf_convenience.c
   libsrc/time_stamp.c
   libsrc/value_conversion.c
   libsrc/voxel_loop.c
   libsrc/hdf_convenience.c
   libsrc/minc_compat.c
   libsrc/minc_simple.c
   )

SET(minc2_LIB_SRCS
   libsrc2/convert.c
   libsrc2/datatype.c
   libsrc2/dimension.c
   libsrc2/free.c
   libsrc2/grpattr.c
   libsrc2/hyper.c
   libsrc2/label.c
   libsrc2/m2util.c
   libsrc2/record.c
   libsrc2/slice.c
   libsrc2/valid.c
   libsrc2/volprops.c
   libsrc2/volume.c
   )


# build the main minc2 library
ADD_LIBRARY(minc2 STATIC ${minc1_LIB_SRCS} ${minc2_LIB_SRCS})
INSTALL(TARGETS minc2 DESTINATION lib)
INSTALL(FILES libsrc/minc.h libsrc/ParseArgv.h 
   libsrc/voxel_loop.h 
   libsrc/nd_loop.h
   libsrc/time_stamp.h
   libsrc/minc_compat.h
   volume_io/Include/volume_io.h
   libsrc2/minc2.h
   libsrc/minc_simple.h
   DESTINATION include
   )
#SET_TARGET_PROPERTIES(minc2 PROPERTIES VERSION 4.2.0 SOVERSION 4 )


# volume_io2
SET(volume_io2_LIB_SRCS
   volume_io/Geometry/colour.c
   volume_io/Geometry/colour_def.c
   volume_io/Geometry/gaussian.c
   volume_io/Geometry/inverse.c
   volume_io/Geometry/newton.c
   volume_io/Geometry/points.c
   volume_io/Geometry/splines.c
   volume_io/Geometry/tensors.c
   volume_io/Geometry/transforms.c
   volume_io/MNI_formats/gen_xf_io.c
   volume_io/MNI_formats/gen_xfs.c
   volume_io/MNI_formats/grid_transforms.c
   volume_io/MNI_formats/mni_io.c
   volume_io/MNI_formats/tag_points.c
   volume_io/MNI_formats/thin_plate_spline.c
   volume_io/Prog_utils/alloc.c
   volume_io/Prog_utils/alloc_check.c
   volume_io/Prog_utils/arrays.c
   volume_io/Prog_utils/files.c
   volume_io/Prog_utils/print.c
   volume_io/Prog_utils/progress.c
   volume_io/Prog_utils/string.c
   volume_io/Prog_utils/time.c
   volume_io/Volumes/evaluate.c
   volume_io/Volumes/get_hyperslab.c
   volume_io/Volumes/input_free.c
   volume_io/Volumes/input_mnc.c
   volume_io/Volumes/input_volume.c
   volume_io/Volumes/multidim_arrays.c
   volume_io/Volumes/output_mnc.c
   volume_io/Volumes/output_volume.c
   volume_io/Volumes/set_hyperslab.c
   volume_io/Volumes/volume_cache.c
   volume_io/Volumes/volumes.c
   )

# build and install volume_io2 library
ADD_LIBRARY(volume_io2 STATIC ${volume_io2_LIB_SRCS})
INSTALL(TARGETS volume_io2 DESTINATION lib)
INSTALL(FILES volume_io/Include/volume_io/alloc.h
	volume_io/Include/volume_io/arrays.h 
	volume_io/Include/volume_io/basic.h 
	volume_io/Include/volume_io/def_math.h 
	volume_io/Include/volume_io/files.h 
	volume_io/Include/volume_io/geom_structs.h 
	volume_io/Include/volume_io/geometry.h 
	volume_io/Include/internal_volume_io.h 
	volume_io/Include/volume_io/multidim.h 
	volume_io/Include/volume_io/progress.h 
	volume_io/Include/volume_io/string_funcs.h 
	volume_io/Include/volume_io/system_dependent.h 
	volume_io/Include/volume_io/transforms.h 
	volume_io/Include/volume_io/vol_io_prototypes.h 
	volume_io/Include/volume_io/volume.h 
	volume_io/Include/volume_io/volume_cache.h
   DESTINATION include/volume_io)

#SET_TARGET_PROPERTIES(volume_io2 PROPERTIES VERSION 4.2.0 SOVERSION 4 )




# 
# ########### next target ###############
# 
# SET(acr_nema_STAT_SRCS
#    conversion/Acr_nema/acr_io.c
#    conversion/Acr_nema/dicom_client_routines.c
#    conversion/Acr_nema/dicom_network.c
#    conversion/Acr_nema/element.c
#    conversion/Acr_nema/file_io.c
#    conversion/Acr_nema/globals.c
#    conversion/Acr_nema/group.c
#    conversion/Acr_nema/message.c
#    conversion/Acr_nema/value_repr.c
#    )
# 
# ADD_LIBRARY(acr_nema STATIC ${acr_nema_STAT_SRCS})
# 
# 
# ########### next target ###############
# 
# SET(voxeltoworld_SRCS
#    progs/coordinates/voxeltoworld.c
#    )
# 
# TARGET_LINK_LIBRARIES(voxeltoworld )
# 
# INSTALL( TARGETS voxeltoworld DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(worldtovoxel_SRCS
#    progs/coordinates/worldtovoxel.c
#    )
# 
# TARGET_LINK_LIBRARIES(worldtovoxel )
# 
# INSTALL( TARGETS worldtovoxel DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minc_modify_header_SRCS
#    progs/minc_modify_header/minc_modify_header.c
#    )
# 
# TARGET_LINK_LIBRARIES(minc_modify_header )
# 
# INSTALL( TARGETS minc_modify_header DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincaverage_SRCS
#    progs/mincaverage/mincaverage.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincaverage )
# 
# INSTALL( TARGETS mincaverage DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minccalc_SRCS
#    progs/minccalc/minccalc.c
#    progs/minccalc/gram.y
#    progs/minccalc/eval.c
#    progs/minccalc/ident.c
#    progs/minccalc/lex.l
#    progs/minccalc/node.c
#    progs/minccalc/optim.c
#    progs/minccalc/scalar.c
#    progs/minccalc/sym.c
#    progs/minccalc/vector.c
#    )
# 
# TARGET_LINK_LIBRARIES(minccalc )
# 
# INSTALL( TARGETS minccalc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincconcat_SRCS
#    progs/mincconcat/mincconcat.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincconcat )
# 
# INSTALL( TARGETS mincconcat DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minccopy_SRCS
#    progs/minccopy/minccopy.c
#    )
# 
# TARGET_LINK_LIBRARIES(minccopy )
# 
# INSTALL( TARGETS minccopy DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincexpand_SRCS
#    progs/mincexpand/mincexpand.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincexpand  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincexpand DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincextract_SRCS
#    progs/mincextract/mincextract.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincextract  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincextract DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincgen_SRCS
#    progs/mincgen/escapes.c
#    progs/mincgen/genlib.c
#    progs/mincgen/getfill.c
#    progs/mincgen/init.c
#    progs/mincgen/load.c
#    progs/mincgen/main.c
#    progs/mincgen/ncgentab.y
#    progs/mincgen/ncgenyy.l
#    )
# 
# TARGET_LINK_LIBRARIES(mincgen  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincgen DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincinfo_SRCS
#    progs/mincinfo/mincinfo.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincinfo  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincinfo DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minclookup_SRCS
#    progs/minclookup/minclookup.c
#    )
# 
# TARGET_LINK_LIBRARIES(minclookup  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS minclookup DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincmakescalar_SRCS
#    progs/mincmakescalar/mincmakescalar.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincmakescalar  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincmakescalar DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincmakevector_SRCS
#    progs/mincmakevector/mincmakevector.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincmakevector  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincmakevector DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincmath_SRCS
#    progs/mincmath/mincmath.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincmath  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincmath DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincresample_SRCS
#    progs/mincresample/mincresample.c
#    progs/mincresample/resample_volumes.c
#    progs/Proglib/convert_origin_to_start.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincresample  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincresample DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincreshape_SRCS
#    progs/mincreshape/mincreshape.c
#    progs/mincreshape/copy_data.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincreshape  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincreshape DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincstats_SRCS
#    progs/mincstats/mincstats.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincstats  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincstats DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minctoraw_SRCS
#    progs/minctoraw/minctoraw.c
#    )
# 
# TARGET_LINK_LIBRARIES(minctoraw  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS minctoraw DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(invert_raw_image_SRCS
#    progs/mincview/invert_raw_image.c
#    )
# 
# TARGET_LINK_LIBRARIES(invert_raw_image  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS invert_raw_image DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincwindow_SRCS
#    progs/mincwindow/mincwindow.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincwindow  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincwindow DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(rawtominc_SRCS
#    progs/rawtominc/rawtominc.c
#    progs/Proglib/convert_origin_to_start.c
#    )
# 
# TARGET_LINK_LIBRARIES(rawtominc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS rawtominc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(transformtags_SRCS
#    progs/xfm/transformtags.c
#    )
# 
# TARGET_LINK_LIBRARIES(transformtags  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS transformtags DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(xfmconcat_SRCS
#    progs/xfm/xfmconcat.c
#    )
# 
# TARGET_LINK_LIBRARIES(xfmconcat  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS xfmconcat DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(xfminvert_SRCS
#    progs/xfm/xfminvert.c
#    )
# 
# TARGET_LINK_LIBRARIES(xfminvert  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS xfminvert DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincconvert_SRCS
#    progs/mincconvert/mincconvert.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincconvert  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincconvert DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincdump_SRCS
#    progs/mincdump/mincdump.c
#    progs/mincdump/vardata.c
#    progs/mincdump/dumplib.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincdump  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincdump DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(ecattominc_SRCS
#    conversion/ecattominc/ecattominc.c
#    conversion/ecattominc/insertblood.c
#    conversion/ecattominc/ecat_file.c
#    conversion/ecattominc/machine_indep.c
#    )
# 
# TARGET_LINK_LIBRARIES(ecattominc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS ecattominc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(minctoecat_SRCS
#    conversion/minctoecat/minctoecat.c
#    conversion/minctoecat/ecat_write.c
#    conversion/minctoecat/machine_indep.c
#    )
# 
# TARGET_LINK_LIBRARIES(minctoecat  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS minctoecat DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mnc2nii_SRCS
#    conversion/nifti1/mnc2nii.c
#    conversion/nifti1/nifti1_io.c
#    conversion/nifti1/znzlib.c
#    )
# 
# TARGET_LINK_LIBRARIES(mnc2nii  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mnc2nii DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(nii2mnc_SRCS
#    conversion/nifti1/nii2mnc.c
#    conversion/nifti1/nifti1_io.c
#    conversion/nifti1/znzlib.c
#    )
# 
# TARGET_LINK_LIBRARIES(nii2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS nii2mnc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(upet2mnc_SRCS
#    conversion/micropet/upet2mnc.c
#    )
# 
# TARGET_LINK_LIBRARIES(upet2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS upet2mnc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(dcm2mnc_SRCS
#    conversion/dcm2mnc/dcm2mnc.c
#    conversion/dcm2mnc/dicom_to_minc.c
#    conversion/dcm2mnc/siemens_to_dicom.c
#    conversion/dcm2mnc/dicom_read.c
#    conversion/dcm2mnc/minc_file.c
#    conversion/dcm2mnc/progress.c
#    conversion/dcm2mnc/string_to_filename.c
#    )
# 
# TARGET_LINK_LIBRARIES(dcm2mnc  ${QT_AND_KDECORE_LIBS} acr_nema )
# 
# INSTALL( TARGETS dcm2mnc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(vff2mnc_SRCS
#    conversion/vff2mnc/vff2mnc.c
#    )
# 
# TARGET_LINK_LIBRARIES(vff2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS vff2mnc DESTINATION bin)
# 
# 
# ########### next target ###############
# 
# SET(mincexample1_SRCS
#    progs/mincexample/mincexample1.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincexample1  ${QT_AND_KDECORE_LIBS} )
# 
# 
# ########### next target ###############
# 
# SET(mincexample2_SRCS
#    progs/mincexample/mincexample2.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincexample2  ${QT_AND_KDECORE_LIBS} )
# 
# 
# ########### install files ###############
# 
# INSTALL(FILES   DESTINATION /include/volume_io )
# INSTALL(FILES   progs/xfm/xfmflip DESTINATION  )
# INSTALL(FILES   progs/mincdiff/mincdiff  
#                 progs/mincedit/mincedit  
#                 progs/mincheader/mincheader  
#                 progs/minchistory/minchistory  
#                 progs/mincpik/mincpik  
#                 progs/mincview/mincview DESTINATION  )
# INSTALL(FILES   DESTINATION /include/acr_nema )
