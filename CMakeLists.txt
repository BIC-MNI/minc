# CMakeFiles.txt for the MINC2 library
#
# Andrew Janke - a.janke@gmail.com
# Vladimir S. FONOV - vladimir.fonov@gmail.com

PROJECT(minc2)

SET(MINC2_PACKAGE_VERSION_MAJOR 2)
SET(MINC2_PACKAGE_VERSION_MINOR 1)
SET(MINC2_PACKAGE_VERSION_PATCH 20)
SET(PACKAGE "minc2")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "minc2")
SET(PACKAGE_VERSION "${MINC2_PACKAGE_VERSION_MAJOR}.${MINC2_PACKAGE_VERSION_MINOR}.${MINC2_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING  "${PACKAGE_NAME} ${PACKAGE_VERSION}")


IF(itk-module)
  # build minimal set for minc4itk library

  SET(BUILD_EZMINC ON)
  SET(BUILD_MINC4ITK OFF)
  SET(BUILD_MINC2 ON)
  SET(BUILD_TOOLS OFF)
  SET(BUILD_ITK_PLUGIN OFF)
  SET(BUILD_MINC4ITK_EXAMPLES OFF)
  SET(BUILD_CONVERTERS OFF)
  SET(BUILD_EZMINC_EXAMPLES OFF)
  
  SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
  
  FIND_PACKAGE(NETCDF REQUIRED)
  # configuring as ITK external module
  SET(USE_ITK_HDF5 ON)  # we made requirement for ITKHDF5
  SET(HDF5_INCLUDE_DIR "${ITKHDF5_INCLUDE_DIRS}/itkhdf5") # a hack?
  SET(HDF5_LIBRARY     ${ITKHDF5_LIBRARIES})
  SET(ZLIB_LIBRARIES   ${ITKZLIB_LIBRARIES})
  
  SET(HAVE_ZLIB 1)# we made requirement for ITKHDF5
ELSE(itk-module)

  CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

  IF(NOT MINC_TOOLKIT_BUILD)
    SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
    ENABLE_TESTING()
    INCLUDE(CTest)
    
    IF(CMAKE_VERSION VERSION_GREATER 2.8.2 )
      INCLUDE(ExternalProject)
      INCLUDE(BuildNETCDF)
      INCLUDE(BuildHDF5)
      SET(ENABLE_SUPERBUILD ON)
    ELSE(CMAKE_VERSION VERSION_GREATER 2.8.2 )
      SET(ENABLE_SUPERBUILD OFF)
    ENDIF(CMAKE_VERSION VERSION_GREATER 2.8.2)
    
    SET(CPACK_GENERATOR TGZ)
    SET(CPACK_PACKAGE_VERSION_MAJOR ${MINC2_PACKAGE_VERSION_MAJOR})
    SET(CPACK_PACKAGE_VERSION_MINOR ${MINC2_PACKAGE_VERSION_MINOR})
    SET(CPACK_PACKAGE_VERSION_PATCH ${MINC2_PACKAGE_VERSION_PATCH})
    
    INCLUDE(CPack)

    FIND_PACKAGE(ITK QUIET)
    
  ENDIF(NOT MINC_TOOLKIT_BUILD)

  #ADD_SUBDIRECTORY( doc )

  OPTION(BUILD_MINC2       "Support minc2 file format" ON)
  OPTION(BUILD_TOOLS       "Build minc tools (mincreshape,mincresample, etc)" ON)
  OPTION(BUILD_CONVERTERS  "Build minc conversion programs (mnc2nii, nii2mnc , dcm2mnc...)" ON)
  OPTION(BUILD_EZMINC      "Build C++ interface library EZminc" ON)
  OPTION(BUILD_SHARED_LIBS "Build minc2 with shared libraries." OFF)

  IF(ENABLE_SUPERBUILD)
    OPTION(USE_SYSTEM_NETCDF "Use System NETCDF > 3.6.3" ON)
  ELSE(ENABLE_SUPERBUILD)
    SET(USE_SYSTEM_NETCDF ON)
  ENDIF(ENABLE_SUPERBUILD)

  IF(BUILD_EZMINC AND ITK_FOUND)
    OPTION(BUILD_MINC4ITK   "Build ITK interface" ON)
  ENDIF(BUILD_EZMINC AND ITK_FOUND)

  IF(BUILD_MINC2 AND NOT MINC_TOOLKIT_BUILD)
    IF(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
      IF(ITKHDF5_LOADED)
        SET(USE_ITK_HDF5 ON)
      ELSE(ITKHDF5_LOADED)
        SET(USE_ITK_HDF5 OFF)
      ENDIF(ITKHDF5_LOADED)
    ELSE(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
      SET(USE_ITK_HDF5 OFF)
      IF(ENABLE_SUPERBUILD)
        OPTION(USE_SYSTEM_HDF5 "Use System HDF5 > 1.8" ON)
      ELSE(ENABLE_SUPERBUILD)
        SET(USE_SYSTEM_HDF5 ON)
      ENDIF(ENABLE_SUPERBUILD)
    ENDIF(ITK_FOUND AND ITK_VERSION_MAJOR VERSION_EQUAL 4) # check if using ITK HDF5 library
  ENDIF(BUILD_MINC2 AND NOT MINC_TOOLKIT_BUILD)

  IF(BUILD_SHARED_LIBS)
    SET(LIBRARY_TYPE SHARED)
    SET(LIBRARY_INSTALL LIBRARY)
  ELSE(BUILD_SHARED_LIBS)
    SET(LIBRARY_TYPE STATIC)
    SET(LIBRARY_INSTALL ARCHIVE)
  ENDIF(BUILD_SHARED_LIBS)


  # external packages
  IF(NOT MINC_TOOLKIT_BUILD)
    IF(USE_SYSTEM_NETCDF AND NOT MINC_TOOLKIT_BUILD)
      FIND_PACKAGE(NETCDF REQUIRED)
    ELSE(USE_SYSTEM_NETCDF AND NOT MINC_TOOLKIT_BUILD)
      build_netcdf(${CMAKE_INSTALL_PREFIX})
    ENDIF(USE_SYSTEM_NETCDF AND NOT MINC_TOOLKIT_BUILD)
  ENDIF(NOT MINC_TOOLKIT_BUILD)


  IF(BUILD_MINC2 AND NOT MINC_TOOLKIT_BUILD)
    IF(USE_ITK_HDF5)
      SET(HDF5_INCLUDE_DIR "${ITKHDF5_INCLUDE_DIRS}/itkhdf5") # a hack?
      SET(HDF5_LIBRARY     ${ITKHDF5_LIBRARIES})
      SET(ZLIB_LIBRARIES   ${ITKZLIB_LIBRARIES})
    ELSE(USE_ITK_HDF5)
      FIND_PACKAGE(ZLIB REQUIRED)
      IF(USE_SYSTEM_HDF5)
        FIND_PACKAGE(HDF5 REQUIRED)
      ELSE(USE_SYSTEM_HDF5)
        build_hdf5(${CMAKE_INSTALL_PREFIX})
      ENDIF(USE_SYSTEM_HDF5)
    ENDIF(USE_ITK_HDF5)
    
    IF(ZLIB_FOUND)
      SET(HAVE_ZLIB 1)
    ELSE(ZLIB_FOUND)
      FIND_PACKAGE(ZLIB)
      IF(ZLIB_FOUND)
        SET(HAVE_ZLIB 1)
      ENDIF(ZLIB_FOUND)
    ENDIF(ZLIB_FOUND)
  ENDIF(BUILD_MINC2 AND NOT MINC_TOOLKIT_BUILD)

  # add for building relocatable library
  IF(UNIX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF(UNIX)

ENDIF(itk-module)

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

IF(BUILD_EZMINC)
  set(MINC2_INCLUDE_DIRS_CONFIG    
    ${CMAKE_CURRENT_SOURCE_DIR}/ezminc
  )
  INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/ezminc
   )
ENDIF(BUILD_EZMINC)

IF(BUILD_MINC4ITK AND BUILD_EZMINC)
  set(MINC2_INCLUDE_DIRS_CONFIG    
    ${CMAKE_CURRENT_SOURCE_DIR}/minc4itk
  )

  INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/minc4itk
   )
ENDIF(BUILD_MINC4ITK AND BUILD_EZMINC)


# netcdf and HDF5

IF(BUILD_MINC2)
  FIND_PACKAGE(ZLIB REQUIRED)
  SET(MINC2 "1")
ELSE(BUILD_MINC2)
  SET(MINC2 "0")
ENDIF(BUILD_MINC2)

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

IF(BUILD_MINC2)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libsrc2 )
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

SET(MINC2_LIBRARY minc)
SET(VOLUME_IO_LIBRARY volume_io)

SET(MINC2_LIBRARIES ${MINC2_LIBRARY} ${NETCDF_LIBRARY})

SET(MINC2_DEPENDENCIES "")

IF(NOT USE_SYSTEM_NETCDF) 
  SET(MINC2_DEPENDENCIES ${MINC2_DEPENDENCIES} NETCDF)
ENDIF(NOT USE_SYSTEM_NETCDF)  


IF(BUILD_MINC2)
  INCLUDE_DIRECTORIES( ${HDF5_INCLUDE_DIR} )

  SET(minc_LIB_SRCS ${minc1_LIB_SRCS} ${minc2_LIB_SRCS})
  SET(minc_HEADERS ${minc1_HEADERS} ${minc2_HEADERS})
  SET(MINC2_LIBRARY minc2)
  SET(MINC2_LIBRARIES ${MINC2_LIBRARY} ${HDF5_LIBRARY} ${NETCDF_LIBRARY} ${ZLIB_LIBRARIES}  m )
  
  SET(VOLUME_IO_LIBRARY volume_io2)
  
  IF(NOT USE_SYSTEM_HDF5)
    SET(MINC2_DEPENDENCIES ${MINC2_DEPENDENCIES} HDF5)
  ENDIF(NOT USE_SYSTEM_HDF5)
ELSE(BUILD_MINC2)
  SET(minc_LIB_SRCS ${minc1_LIB_SRCS} )
  SET(minc_HEADERS ${minc1_HEADERS} ${minc2_HEADERS})
ENDIF(BUILD_MINC2)


IF(NOT itk-module)

  ADD_LIBRARY(${MINC2_LIBRARY} ${LIBRARY_TYPE} ${minc_LIB_SRCS} )
  TARGET_LINK_LIBRARIES(${MINC2_LIBRARY} ${NETCDF_LIBRARY} ${HDF5_LIBRARY} ${ZLIB_LIBRARIES} m )

  ADD_LIBRARY(${VOLUME_IO_LIBRARY} ${LIBRARY_TYPE} ${volume_io_LIB_SRCS})

  SET_TARGET_PROPERTIES(${MINC2_LIBRARY}     PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${MINC2_PACKAGE_VERSION_MAJOR})
  SET_TARGET_PROPERTIES(${VOLUME_IO_LIBRARY} PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${MINC2_PACKAGE_VERSION_MAJOR})
  
  ADD_DEPENDENCIES(${MINC2_LIBRARY} ${MINC2_DEPENDENCIES}) 
  ADD_DEPENDENCIES(${VOLUME_IO_LIBRARY} ${MINC2_LIBRARY})
  
  INSTALL(TARGETS ${MINC2_LIBRARY} ${LIBRARY_INSTALL} DESTINATION lib)
  INSTALL(TARGETS ${VOLUME_IO_LIBRARY} ${LIBRARY_INSTALL} DESTINATION lib)
  
  INSTALL(FILES 
      ${minc2_HEADERS}
    DESTINATION 
      include
    )
  
  INSTALL(FILES
    ${volume_io_HEADERS}
    DESTINATION include/volume_io)


  IF(BUILD_TOOLS)
    ADD_SUBDIRECTORY( progs )
  ENDIF(BUILD_TOOLS)

  IF(BUILD_CONVERTERS)
    ADD_SUBDIRECTORY( conversion )
  ENDIF(BUILD_CONVERTERS)

  IF(BUILD_EZMINC)
    ADD_SUBDIRECTORY( ezminc )
  ENDIF(BUILD_EZMINC)

  IF(BUILD_MINC4ITK AND BUILD_EZMINC)
    ADD_SUBDIRECTORY( minc4itk )
  ENDIF(BUILD_MINC4ITK AND BUILD_EZMINC)

  # config for the build directory
  set(MINC2_USE_FILE_CONFIG  ${CMAKE_CURRENT_BINARY_DIR}/UseMINC2.cmake)

  set(MINC2_INCLUDE_DIRS_CONFIG
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libsrc
    ${CMAKE_CURRENT_SOURCE_DIR}/volume_io/Include
  )

  IF(BUILD_MINC2)
    set(MINC2_INCLUDE_DIRS_CONFIG
      ${MINC2_INCLUDE_DIRS_CONFIG}
      ${CMAKE_CURRENT_SOURCE_DIR}/libsrc2
    )
  ENDIF(BUILD_MINC2)

  IF(BUILD_EZMINC)
    SET(EZMINC_LIBRARIES minc_io ${MINC2_LIBRARIES})
  ENDIF(BUILD_EZMINC)

  IF(BUILD_MINC4ITK)
    SET(MINC4ITK_LIBRARIES minc4itk ${EZMINC_LIBRARIES})
  ENDIF(BUILD_MINC4ITK)

  set(MINC2_LIBRARY_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR})

  configure_file(MINC2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MINC2Config.cmake @ONLY )

  configure_file(UseMINC2.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/UseMINC2.cmake @ONLY)

  # config for install dir
  set(MINC2_USE_FILE_CONFIG "${CMAKE_INSTALL_PREFIX}/lib/UseMINC2.cmake")

  set(MINC2_INCLUDE_DIRS_CONFIG
    ${CMAKE_INSTALL_PREFIX}/include
  )

  set(MINC2_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)

  configure_file(MINC2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/MINC2Config.cmake @ONLY )

  configure_file(UseMINC2.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseMINC2.cmake @ONLY)

  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseMINC2.cmake ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/MINC2Config.cmake
    DESTINATION lib
    COMPONENT Development)
  # testing

ELSE(NOT itk-module)

  SET(ITKIOMINC_LIBRARIES ITKIOMINC ${ITKHDF5_LIBRARIES} ${NETCDF_LIBRARY} ${ITKZLIB_LIBRARIES})
  SET(ITKIOMINC_NO_SRC ON)
  SET(ITKIOMINC_SYSTEM_INCLUDE_DIRS ${NETCDF_INCLUDE_DIR}) 
  
  #SET(ITKIOMINC_INCLUDE_DIRS ${NETCDF_INCLUDE_DIR})
  SET(ITKIOMINC_THIRD_PARTY ON)
  
  IF(BUILD_MINC2)
    ADD_DEFINITIONS( -DMINC2 )
  ENDIF(BUILD_MINC2)
  
  INCLUDE_DIRECTORIES( ezminc minc4itk )
  
  ADD_LIBRARY(ITKIOMINC 
    # EZ Minc
    ezminc/minc_1_rw.cpp
    ezminc/minc_1_simple_rw.cpp  
    
    # minc4itk
    minc4itk/itkMincImageIO.cxx
    minc4itk/itkMincImageIOFactory.cxx
  
    # volume io
    ${volume_io_LIB_SRCS} 
    
    # minc low level
    ${minc_LIB_SRCS} )
  
  target_link_libraries(ITKIOMINC
    ${ITKIOImageBase_LIBRARIES}
    ${ITKHDF5_LIBRARIES}
    ${NETCDF_LIBRARY}
    ${ITKZLIB_LIBRARIES}
  )
  
  itk_module_target(ITKIOMINC NO_INSTALL)
  
  if(NOT ITK_INSTALL_NO_LIBRARIES)
    install(TARGETS ITKIOMINC
      EXPORT ${ITKIOMINC_INSTALL_EXPORT_NAME}
      RUNTIME DESTINATION ${ITK_INSTALL_RUNTIME_DIR} COMPONENT RuntimeLibraries
      LIBRARY DESTINATION ${ITK_INSTALL_LIBRARY_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${ITK_INSTALL_ARCHIVE_DIR} COMPONENT Development
      )
  endif(NOT ITK_INSTALL_NO_LIBRARIES)

  itk_module_impl()
ENDIF(NOT itk-module)

IF(BUILD_TESTING AND NOT itk-module)
  ADD_SUBDIRECTORY( testdir )
ENDIF(BUILD_TESTING AND NOT itk-module)
