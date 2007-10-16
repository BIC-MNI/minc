# CMakeFiles.txt for the MINC2 library
#
# Andrew Janke - a.janke@gmail.com

PROJECT(libminc2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")


#ADD_SUBDIRECTORY( testdir )
#ADD_SUBDIRECTORY( doc )


# Packaging defines
SET(CPACK_GENERATOR TGZ)
SET(CPACK_PACKAGE_VERSION_MAJOR 2)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 14)
INCLUDE(CPack)


SET(PACKAGE "minc")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "minc")
SET(PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


# yes, build as minc2
SET(MINC2 "1")
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# aliases
SET(VERSION "${PACKAGE_VERSION}")

# progs needed to build
SET(FLEX_FIND_REQUIRED)
SET(BISON_FIND_REQUIRED)
FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

# check for prereqs
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(float.h HAVE_FLOAT_H)

# netcdf and HDF5
SET(NETCDF_FIND_REQUIRED TRUE)
SET(HDF5_FIND_REQUIRED TRUE)
FIND_PACKAGE(HDF5)
FIND_PACKAGE(NETCDF)
INCLUDE_DIRECTORIES( ${NETCDF_INCLUDE_DIR} ${HDF5_INCLUDE_DIR} )

# config files for build
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_SOURCE_DIR}/config.h)

# others
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/epm-header.in ${CMAKE_SOURCE_DIR}/epm-header)

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

SET_TARGET_PROPERTIES(minc2 PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})


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

SET_TARGET_PROPERTIES(volume_io2 PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})


# now build the progs subdir
ADD_SUBDIRECTORY( progs )


