FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

IF(NOT ITK_LIBRARIES)
  MESSAGE( FATAL_ERROR "ITK libraries are not found!")
ENDIF(NOT ITK_LIBRARIES)

IF(NOT ITK_INCLUDE_DIRS)
  MESSAGE( FATAL_ERROR "ITK include directories are not found!")
ENDIF(NOT ITK_INCLUDE_DIRS	)

IF(BUILD_SHARED_LIBS AND NOT ITK_BUILD_SHARED)
  message(WARNING "Building Shared library but ITK is linked statically!")
ENDIF(BUILD_SHARED_LIBS AND NOT ITK_BUILD_SHARED)

OPTION(BUILD_ITK_PLUGIN    "Build ITK plugin" OFF)
OPTION(BUILD_MINC4ITK_EXAMPLES   "Build minc4itk examples" ON)

LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
	${ITK_INCLUDE_DIRS}
	)
    
SET( MINC4ITK_HEADERS
    itkMincImageIOFactory.h
    minc_general_transform.h
    minc_helpers.h
    itkMincImageIO.h
    )

SET( MINC4ITK_SRC 
    minc_helpers.cxx
    itkMincImageIO.cxx
    itkMincImageIOFactory.cxx
  )

ADD_LIBRARY( minc4itk ${MINC4ITK_HEADERS} ${MINC4ITK_SRC})

TARGET_LINK_LIBRARIES( minc4itk 
    minc_io
    ${ITK_LIBRARIES}
    itkvnl_algo 
    itkvnl 
    ${VOLUME_IO_LIBRARY}
    ${MINC2_LIBRARIES}
    m 
    z )

#VF ITK plugin is disabled for now
IF(BUILD_ITK_PLUGIN)
message(WARNING "ITK IO plugin might not work as expected!")
ADD_LIBRARY( minc4itk_plugin SHARED minc4itk_plugin.cxx)
TARGET_LINK_LIBRARIES(  minc4itk_plugin minc4itk)
INSTALL(TARGETS minc4itk_plugin LIBRARY DESTINATION lib)
ENDIF(BUILD_ITK_PLUGIN)

INSTALL(TARGETS minc4itk ${LIBRARY_INSTALL} DESTINATION lib)
INSTALL(FILES  ${MINC4ITK_HEADERS} DESTINATION include)

IF(BUILD_MINC4ITK_EXAMPLES)
add_subdirectory(examples)
ENDIF(BUILD_MINC4ITK_EXAMPLES)