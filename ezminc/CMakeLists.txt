LINK_LIBRARIES(
    m 
    z )

IF(USE_MINC2)
  ADD_DEFINITIONS( -DMINC2 )
  LINK_LIBRARIES(volume_io2 minc2 netcdf hdf5 z)
ELSE(USE_MINC2)
  LINK_LIBRARIES(volume_io minc netcdf)
ENDIF(USE_MINC2)


INCLUDE_DIRECTORIES( 
  ${VOLUME_TOOLS_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)


SET( MINC_IO_HEADERS 
    minc_io_exceptions.h 
    minc_io_fixed_vector.h  
    minc_io_simple_volume.h
    minc_1_rw.h
    minc_1_simple.h
    minc_1_simple_rw.h
    minc_io_4d_volume.h
   )
   
SET( MINC_IO_SRC 
    minc_1_rw.cpp
    minc_1_simple_rw.cpp
  )

ADD_LIBRARY( minc_io ${MINC_IO_HEADERS} ${MINC_IO_SRC})


INSTALL(TARGETS minc_io ARCHIVE DESTINATION lib)
INSTALL(FILES  ${MINC_IO_HEADERS} DESTINATION include)
