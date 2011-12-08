
IF(BUILD_MINC2)
  ADD_DEFINITIONS( -DMINC2 )
ENDIF(BUILD_MINC2)


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
