IF(NOT ITK_LIBRARIES)
  MESSAGE( FATAL_ERROR "ITK libraries are not found!")
ENDIF(NOT ITK_LIBRARIES)

IF(NOT ITK_INCLUDE_DIRS)
  MESSAGE( FATAL_ERROR "ITK include directories are not found!")
ENDIF(NOT ITK_INCLUDE_DIRS	)

#OPTION(BUILD_ITK_PLUGIN    "Build ITK plugin" OFF)

LINK_LIBRARIES(
    ${ITK_LIBRARIES}
    itkvnl_algo 
    itkvnl 
    m 
    z )

INCLUDE_DIRECTORIES(
	${ITK_INCLUDE_DIRS}
	)
    

IF(USE_MINC2)
  ADD_DEFINITIONS( -DMINC2 )
  LINK_LIBRARIES(volume_io2 minc2 netcdf hdf5 z)
ELSE(USE_MINC2)
  LINK_LIBRARIES(volume_io minc netcdf)
ENDIF(USE_MINC2)

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
    m 
    z )

#VF ITK plugin is disabled for now
#IF(BUILD_ITK_PLUGIN)
#ADD_LIBRARY( minc4itk_plugin SHARED minc4itk_plugin.cxx)
#TARGET_LINK_LIBRARIES(  minc4itk_plugin minc4itk)
#INSTALL(TARGETS minc4itk_plugin LIBRARY DESTINATION lib)
#ENDIF(BUILD_ITK_PLUGIN)

INSTALL(TARGETS minc4itk ARCHIVE DESTINATION lib)
INSTALL(FILES  ${MINC4ITK_HEADERS} DESTINATION include)
