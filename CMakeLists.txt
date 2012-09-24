# CMakeFiles.txt for the libminc library
#
# Andrew Janke - a.janke@gmail.com
# Vladimir S. FONOV - vladimir.fonov@gmail.com

PROJECT(libminc)

SET(LIBMINC_PACKAGE_VERSION_MAJOR 2)
SET(LIBMINC_PACKAGE_VERSION_MINOR 2)
SET(LIBMINC_PACKAGE_VERSION_PATCH 00)
SET(PACKAGE "libminc")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "libminc")
SET(PACKAGE_VERSION "${LIBMINC_PACKAGE_VERSION_MAJOR}.${LIBMINC_PACKAGE_VERSION_MINOR}.${LIBMINC_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING  "${PACKAGE_NAME} ${PACKAGE_VERSION}")
ENABLE_TESTING()
INCLUDE(CTest)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(MINC_TOOLKIT_BUILD)
  SET(LIBMINC_EXTERNALLY_CONFIGURED ON)
ENDIF(MINC_TOOLKIT_BUILD)

IF(NOT LIBMINC_EXTERNALLY_CONFIGURED)
  SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
  
  SET(CPACK_GENERATOR TGZ)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${LIBMINC_PACKAGE_VERSION_MAJOR})
  SET(CPACK_PACKAGE_VERSION_MINOR ${LIBMINC_PACKAGE_VERSION_MINOR})
  SET(CPACK_PACKAGE_VERSION_PATCH ${LIBMINC_PACKAGE_VERSION_PATCH})
  
  OPTION(LIBMINC_BUILD_SHARED_LIBS "Build libminc with shared libraries." OFF)
  
  INCLUDE(CPack)
  
  OPTION(LIBMINC_BUILD_V2                "Support minc2 file format" ON)
  OPTION(LIBMINC_BUILD_EZMINC_EXAMPLES   "Build EZminc examples" ON)
  
ENDIF(NOT LIBMINC_EXTERNALLY_CONFIGURED)

IF(LIBMINC_BUILD_V2 AND NOT LIBMINC_EXTERNALLY_CONFIGURED)
  IF(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
    IF(ITKHDF5_LOADED)
      SET(USE_ITK_HDF5 ON)
    ELSE(ITKHDF5_LOADED)
      SET(USE_ITK_HDF5 OFF)
    ENDIF(ITKHDF5_LOADED)
  ELSE(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
    SET(USE_ITK_HDF5 OFF)
  ENDIF(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
ENDIF(LIBMINC_BUILD_V2 AND NOT LIBMINC_EXTERNALLY_CONFIGURED)

IF(LIBMINC_BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE SHARED)
  SET(LIBRARY_INSTALL LIBRARY)
ELSE(LIBMINC_BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE STATIC)
  SET(LIBRARY_INSTALL ARCHIVE)
ENDIF(LIBMINC_BUILD_SHARED_LIBS)

# external packages
IF(NOT LIBMINC_EXTERNALLY_CONFIGURED)
  FIND_PACKAGE(NETCDF REQUIRED)
ENDIF(NOT LIBMINC_EXTERNALLY_CONFIGURED)


IF(LIBMINC_BUILD_V2 AND NOT LIBMINC_EXTERNALLY_CONFIGURED)
  IF(USE_ITK_HDF5)
    SET(HDF5_INCLUDE_DIR "${ITKHDF5_INCLUDE_DIRS}/itkhdf5") # a hack?
    SET(HDF5_LIBRARY     ${ITKHDF5_LIBRARIES})
    SET(ZLIB_LIBRARY   ${ITKZLIB_LIBRARIES})
  ELSE(USE_ITK_HDF5)
    FIND_PACKAGE(ZLIB REQUIRED)
    FIND_PACKAGE(HDF5 REQUIRED)
  ENDIF(USE_ITK_HDF5)
  
  IF(ZLIB_FOUND)
    SET(HAVE_ZLIB 1)
  ELSE(ZLIB_FOUND)
    FIND_PACKAGE(ZLIB)
    IF(ZLIB_FOUND)
      SET(HAVE_ZLIB 1)
    ENDIF(ZLIB_FOUND)
  ENDIF(ZLIB_FOUND)
ENDIF(LIBMINC_BUILD_V2 AND NOT LIBMINC_EXTERNALLY_CONFIGURED)

# add for building relocatable library
IF(UNIX)
  SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fPIC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(UNIX)

# check for prereqs
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(mkstemp  HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(tmpnam   HAVE_TMPNAM)
CHECK_FUNCTION_EXISTS(tempnam  HAVE_TEMPNAM)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)
CHECK_FUNCTION_EXISTS(popen    HAVE_POPEN)
CHECK_FUNCTION_EXISTS(fork     HAVE_WORKING_FORK)
CHECK_FUNCTION_EXISTS(vfork    HAVE_WORKING_VFORK)
CHECK_FUNCTION_EXISTS(fdopen   HAVE_FDOPEN)
CHECK_FUNCTION_EXISTS(strdup   HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(getpwnam HAVE_GETPWNAM) 
CHECK_FUNCTION_EXISTS(select   HAVE_SELECT)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR) 
CHECK_FUNCTION_EXISTS(sysconf  HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(system   HAVE_SYSTEM)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(float.h     HAVE_FLOAT_H)
CHECK_INCLUDE_FILES(sys/dir.h   HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILES(sys/ndir.h  HAVE_SYS_NDIR_H)
CHECK_INCLUDE_FILES(sys/stat.h  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/wait.h  HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES(values.h    HAVE_VALUES_H)
CHECK_INCLUDE_FILES(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(dirent.h    HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(memory.h    HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(stdlib.h    HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(fcntl.h     HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(dlfcn.h     HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(vfork.h     HAVE_VFORK_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(string.h    HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h   HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(pwd.h       HAVE_PWD_H)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# aliases
SET(VERSION "${PACKAGE_VERSION}")

IF(LIBMINC_BUILD_EZMINC)
  set(LIBMINC_INCLUDE_DIRS_CONFIG    
    ${CMAKE_CURRENT_SOURCE_DIR}/ezminc
  )
  INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/ezminc
   )
ENDIF(LIBMINC_BUILD_EZMINC)

# netcdf and HDF5

IF(LIBMINC_BUILD_V2)
  FIND_PACKAGE(ZLIB REQUIRED)
  SET(MINC2 "1")
ELSE(BUILD_MINC2)
  SET(MINC2 "0")
ENDIF(LIBMINC_BUILD_V2)

# config files for build
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# others
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/epm-header.in ${CMAKE_CURRENT_BINARY_DIR}/epm-header)

# set the master INCLUDE directories
INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/libsrc
   ${CMAKE_CURRENT_SOURCE_DIR}/volume_io/Include
   )

INCLUDE_DIRECTORIES( ${NETCDF_INCLUDE_DIR} )

IF(LIBMINC_BUILD_V2)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libsrc2 )
ENDIF(LIBMINC_BUILD_V2)

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
   libsrc/read_file_names.c
   )

SET(minc1_HEADERS
  libsrc/minc.h 
  libsrc/ParseArgv.h 
  libsrc/voxel_loop.h 
  libsrc/nd_loop.h
  libsrc/time_stamp.h
  libsrc/minc_compat.h
  volume_io/Include/volume_io.h
  libsrc/minc_simple.h
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

SET(minc2_HEADERS
  libsrc2/minc2.h
)

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

SET(volume_io_HEADERS
    volume_io/Include/volume_io/alloc.h
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
)

SET(LIBMINC_LIBRARY minc)
SET(VOLUME_IO_LIBRARY volume_io)

SET(LIBMINC_LIBRARIES ${LIBMINC_LIBRARY} ${NETCDF_LIBRARY})

#SET(LIBMINC_DEPENDENCIES "")

IF(LIBMINC_BUILD_V2)

  INCLUDE_DIRECTORIES( ${HDF5_INCLUDE_DIR} )

  SET(minc_LIB_SRCS ${minc1_LIB_SRCS} ${minc2_LIB_SRCS})
  SET(minc_HEADERS ${minc1_HEADERS} ${minc2_HEADERS})
  SET(LIBMINC_LIBRARY minc2)
  SET(LIBMINC_LIBRARIES ${LIBMINC_LIBRARY} ${HDF5_LIBRARY} ${NETCDF_LIBRARY} ${ZLIB_LIBRARY}  m )
  SET(VOLUME_IO_LIBRARY volume_io2)
  
ELSE(LIBMINC_BUILD_V2)
  SET(minc_LIB_SRCS ${minc1_LIB_SRCS} )
  SET(minc_HEADERS ${minc1_HEADERS} ${minc2_HEADERS})
ENDIF(LIBMINC_BUILD_V2)


ADD_LIBRARY(${LIBMINC_LIBRARY} ${LIBRARY_TYPE} ${minc_LIB_SRCS} )
TARGET_LINK_LIBRARIES(${LIBMINC_LIBRARY} ${NETCDF_LIBRARY} ${HDF5_LIBRARY} ${ZLIB_LIBRARY} m )

ADD_LIBRARY(${VOLUME_IO_LIBRARY} ${LIBRARY_TYPE} ${volume_io_LIB_SRCS})
TARGET_LINK_LIBRARIES(${VOLUME_IO_LIBRARY} ${LIBMINC_LIBRARY})

SET_TARGET_PROPERTIES(${LIBMINC_LIBRARY}     PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${LIBMINC_PACKAGE_VERSION_MAJOR})
SET_TARGET_PROPERTIES(${VOLUME_IO_LIBRARY} PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${LIBMINC_PACKAGE_VERSION_MAJOR})

ADD_DEPENDENCIES(${VOLUME_IO_LIBRARY} ${LIBMINC_LIBRARY})

INSTALL(TARGETS ${LIBMINC_LIBRARY}     ${LIBRARY_INSTALL} DESTINATION lib)
INSTALL(TARGETS ${VOLUME_IO_LIBRARY} ${LIBRARY_INSTALL} DESTINATION lib)
INSTALL(FILES   ${minc_HEADERS}      DESTINATION  include  )
INSTALL(FILES   ${volume_io_HEADERS} DESTINATION include/volume_io)

ADD_SUBDIRECTORY( ezminc )

# config for the build directory
set(LIBMINC_USE_FILE_CONFIG  ${CMAKE_CURRENT_BINARY_DIR}/UseLIBMINC.cmake)

set(LIBMINC_INCLUDE_DIRS_CONFIG
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/libsrc
  ${CMAKE_CURRENT_SOURCE_DIR}/volume_io/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/ezminc
)

IF(LIBMINC_BUILD_V2)
  set(LIBMINC_INCLUDE_DIRS_CONFIG
    ${LIBMINC_INCLUDE_DIRS_CONFIG}
    ${CMAKE_CURRENT_SOURCE_DIR}/libsrc2
  )
ENDIF(LIBMINC_BUILD_V2)

SET(EZMINC_LIBRARIES minc_io ${LIBMINC_LIBRARIES})
set(LIBMINC_LIBRARY_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ezminc)
set(LIBMINC_LIBRARIES_CONFIG ${LIBMINC_LIBRARIES})


configure_file(LIBMINCConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/LIBMINCConfig.cmake @ONLY )

configure_file(UseLIBMINC.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UseLIBMINC.cmake @ONLY)

# config for install dir
set(LIBMINC_USE_FILE_CONFIG "${CMAKE_INSTALL_PREFIX}/lib/UseLIBMINC.cmake")
set(LIBMINC_INCLUDE_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/include )
set(LIBMINC_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)

#fix for superbuild install
IF(SUPERBUILD_STAGING_PREFIX)

  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" LIBMINC_INCLUDE_DIRS_CONFIG "${LIBMINC_INCLUDE_DIRS_CONFIG}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" LIBMINC_LIBRARY_DIRS_CONFIG "${LIBMINC_LIBRARY_DIRS_CONFIG}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" LIBMINC_LIBRARIES_CONFIG    "${LIBMINC_LIBRARIES_CONFIG}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" EZMINC_LIBRARIES            "${EZMINC_LIBRARIES}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" NETCDF_INCLUDE_DIR          "${NETCDF_INCLUDE_DIR}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" HDF5_INCLUDE_DIR            "${HDF5_INCLUDE_DIR}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" NETCDF_LIBRARY              "${NETCDF_LIBRARY}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" HDF5_LIBRARY                "${HDF5_LIBRARY}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" ZLIB_LIBRARIES              "${ZLIB_LIBRARIES}")
  
ENDIF(SUPERBUILD_STAGING_PREFIX)

configure_file(LIBMINCConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LIBMINCConfig.cmake @ONLY )

configure_file(UseLIBMINC.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseLIBMINC.cmake @ONLY)

INSTALL(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseLIBMINC.cmake 
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LIBMINCConfig.cmake
  DESTINATION lib
  COMPONENT Development)

# testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(testdir)
ENDIF(BUILD_TESTING)
