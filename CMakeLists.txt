# CMakeFiles.txt for the MINC2 library
#
# Andrew Janke - a.janke@gmail.com

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libminc2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")


#ADD_SUBDIRECTORY( testdir )
#ADD_SUBDIRECTORY( doc )


# Packaging defines
SET(CPACK_GENERATOR TGZ)
SET(CPACK_PACKAGE_VERSION_MAJOR 2)
SET(CPACK_PACKAGE_VERSION_MINOR 1)
SET(CPACK_PACKAGE_VERSION_PATCH 1)
INCLUDE(CPack)


SET(PACKAGE "minc")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "minc")
SET(PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

OPTION(BUILD_MINC2      "Support minc2 file format" ON)
OPTION(BUILD_TOOLS      "Build minc tools (mincreshape,mincresample, etc)" ON)
OPTION(BUILD_CONVERTERS "Build minc conversion programs (mnc2nii, nii2mnc , dcm2mnc...)" ON)
OPTION(BUILD_EZMINC     "Build C++ interface library EZminc" ON)
IF(BUILD_EZMINC)
OPTION(BUILD_MINC4ITK   "Build ITK interface" ON)
ENDIF(BUILD_EZMINC)


ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# aliases
SET(VERSION "${PACKAGE_VERSION}")

# check for prereqs
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(float.h HAVE_FLOAT_H)

# netcdf and HDF5
FIND_PACKAGE(NETCDF REQUIRED)
INCLUDE_DIRECTORIES( ${NETCDF_INCLUDE_DIR} )

IF(BUILD_MINC2)
  FIND_PACKAGE(HDF5 REQUIRED)
  FIND_PACKAGE(ZLIB REQUIRED)
  SET(MINC2 "1")
  INCLUDE_DIRECTORIES( ${HDF5_INCLUDE_DIR} )
ENDIF(BUILD_MINC2)

# config files for build
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_SOURCE_DIR}/config.h)

# others
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/epm-header.in ${CMAKE_SOURCE_DIR}/epm-header)

# set the master INCLUDE directories
INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_SOURCE_DIR}/libsrc
   ${CMAKE_SOURCE_DIR}/volume_io/Include
   )

IF(BUILD_MINC2)
  INCLUDE_DIRECTORIES(
   ${CMAKE_SOURCE_DIR}/libsrc2
   )
ENDIF(BUILD_MINC2)


# some variables
SET(minc1_LIB_SRCS
   libsrc/ParseArgv.c
   libsrc/dim_conversion.c
   libsrc/image_conversion.c
   libsrc/minc_convenience.c
   libsrc/minc_error.c
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

SET(minc_LIB minc)

IF(BUILD_MINC2)
  SET(minc_LIB_SRCS ${minc1_LIB_SRCS} ${minc2_LIB_SRCS})
  SET(minc_LIB minc2)
  ADD_LIBRARY(minc2 STATIC ${minc_LIB_SRCS} )

  TARGET_LINK_LIBRARIES(minc2 ${NETCDF_LIBRARY} ${HDF5_LIBRARY} ${ZLIB_LIBRARIES} m )

  INSTALL(TARGETS minc2 ARCHIVE DESTINATION lib)

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

ELSE(BUILD_MINC2)

    SET(minc_LIB_SRCS ${minc1_LIB_SRCS} )
    SET(minc_LIB minc)
    ADD_LIBRARY(minc STATIC ${minc1_LIB_SRCS} )
    TARGET_LINK_LIBRARIES(minc ${NETCDF_LIBRARY} )
    INSTALL(TARGETS minc ARCHIVE DESTINATION lib)

    INSTALL(FILES libsrc/minc.h libsrc/ParseArgv.h 
      libsrc/voxel_loop.h 
      libsrc/nd_loop.h
      libsrc/time_stamp.h
      libsrc/minc_compat.h
      volume_io/Include/volume_io.h
      libsrc/minc_simple.h
      DESTINATION include
      )

    SET_TARGET_PROPERTIES(minc PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})

ENDIF(BUILD_MINC2)

# build the main minc2 library

# volume_io2
SET(volume_io_LIB_SRCS
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

IF(BUILD_MINC2)
  # build and install volume_io2 library
  ADD_LIBRARY(volume_io2 STATIC ${volume_io_LIB_SRCS})
  INSTALL(TARGETS volume_io2 ARCHIVE DESTINATION lib)
  SET_TARGET_PROPERTIES(volume_io2 PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})
  SET( volume_io_LIB volume_io2)

ELSE(BUILD_MINC2)
  ADD_LIBRARY(volume_io STATIC ${volume_io_LIB_SRCS})
  INSTALL(TARGETS volume_io ARCHIVE DESTINATION lib)
  SET_TARGET_PROPERTIES(volume_io PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})
  SET( volume_io_LIB volume_io)
ENDIF(BUILD_MINC2)

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

IF(BUILD_TOOLS)
# now build the progs subdir
ADD_SUBDIRECTORY( progs )
ENDIF(BUILD_TOOLS)

IF(BUILD_CONVERTERS)
# and then the conversion subdir
ADD_SUBDIRECTORY( conversion )
ENDIF(BUILD_CONVERTERS)

IF(BUILD_EZMINC)
ADD_SUBDIRECTORY( ezminc )
ENDIF(BUILD_EZMINC)

IF(BUILD_MINC4ITK AND BUILD_EZMINC)
ADD_SUBDIRECTORY( minc4itk )
ENDIF(BUILD_MINC4ITK AND BUILD_EZMINC)