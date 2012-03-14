# CMakeFiles.txt for the MINC2 progs
#
# Andrew Janke - a.janke@gmail.com

#SET(CMAKE_MODULE_PATH "cmake-modules")

#LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/..)
INCLUDE_DIRECTORIES(Proglib)

LINK_LIBRARIES( ${MINC2_LIBRARIES} )
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

#SET(BISON_FIND_REQUIRED)
#SET(FLEX_FIND_REQUIRED)

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

# all the progs
ADD_EXECUTABLE(invert_raw_image mincview/invert_raw_image.c)
ADD_EXECUTABLE(mincaverage mincaverage/mincaverage.c)
TARGET_LINK_LIBRARIES(mincaverage m)

IF(BISON_FOUND AND FLEX_FOUND)
  include_directories(${CMAKE_CURRENT_BINARY_DIR} minccalc)
  
  BISON_TARGET(gram ${CMAKE_CURRENT_SOURCE_DIR}/minccalc/gram.y ${CMAKE_CURRENT_BINARY_DIR}/gram.c COMPILE_FLAGS "--debug")
  FLEX_TARGET(lex ${CMAKE_CURRENT_SOURCE_DIR}/minccalc/lex.l ${CMAKE_CURRENT_BINARY_DIR}/lex.c  )
  #ADD_FLEX_BISON_DEPENDENCY(gram lex)

  ADD_EXECUTABLE(minccalc 
                  minccalc/minccalc.c
                  minccalc/eval.c
                  minccalc/ident.c
                  minccalc/node.c
                  minccalc/optim.c
                  minccalc/scalar.c
                  minccalc/sym.c
                  minccalc/vector.c
                  ${FLEX_lex_OUTPUTS}
                  ${BISON_gram_OUTPUTS}
                 )

  TARGET_LINK_LIBRARIES(minccalc ${FLEX_LIBRARIES} m)

  INSTALL( TARGETS minccalc  DESTINATION bin)

  include_directories(${CMAKE_CURRENT_BINARY_DIR} mincgen)
  BISON_TARGET(ncgentab ${CMAKE_CURRENT_SOURCE_DIR}/mincgen/ncgentab.y ${CMAKE_CURRENT_BINARY_DIR}/ncgentab.c)
  FLEX_TARGET(ncgenyy ${CMAKE_CURRENT_SOURCE_DIR}/mincgen/ncgenyy.l ${CMAKE_CURRENT_BINARY_DIR}/ncgenyy.c)

  ADD_EXECUTABLE(mincgen mincgen/main.c
                            ${BISON_ncgentab_OUTPUTS}
                            ${FLEX_ncgenyy_OUTPUTS}
                            mincgen/escapes.c
                            mincgen/genlib.c
                            mincgen/getfill.c
                            mincgen/init.c
                            mincgen/load.c)
  INSTALL( TARGETS mincgen    DESTINATION bin)

  TARGET_LINK_LIBRARIES(mincgen ${FLEX_LIBRARIES})

ENDIF(BISON_FOUND AND FLEX_FOUND)

ADD_EXECUTABLE(mincconcat mincconcat/mincconcat.c)
ADD_EXECUTABLE(mincconvert mincconvert/mincconvert.c)
ADD_EXECUTABLE(minccopy minccopy/minccopy.c)

ADD_EXECUTABLE(mincdump mincdump/mincdump.c
                            mincdump/vardata.c
                            mincdump/dumplib.c)
                            
ADD_EXECUTABLE(mincexample1 mincexample/mincexample1.c)
ADD_EXECUTABLE(mincexample2 mincexample/mincexample2.c)
ADD_EXECUTABLE(mincexpand mincexpand/mincexpand.c)
ADD_EXECUTABLE(mincextract mincextract/mincextract.c)
ADD_EXECUTABLE(mincinfo mincinfo/mincinfo.c)
ADD_EXECUTABLE(minclookup minclookup/minclookup.c)
TARGET_LINK_LIBRARIES(minclookup m)

ADD_EXECUTABLE(mincmakescalar mincmakescalar/mincmakescalar.c)
TARGET_LINK_LIBRARIES(mincmakescalar m)

ADD_EXECUTABLE(mincmakevector mincmakevector/mincmakevector.c)
ADD_EXECUTABLE(mincmath mincmath/mincmath.c)
TARGET_LINK_LIBRARIES(mincmath m)

ADD_EXECUTABLE(minc_modify_header minc_modify_header/minc_modify_header.c)

ADD_EXECUTABLE(mincresample mincresample/mincresample.c
                               mincresample/resample_volumes.c
                               Proglib/convert_origin_to_start.c)
TARGET_LINK_LIBRARIES(mincresample ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(mincreshape mincreshape/mincreshape.c
                              mincreshape/copy_data.c)

ADD_EXECUTABLE(mincstats mincstats/mincstats.c)
TARGET_LINK_LIBRARIES(mincstats m)

ADD_EXECUTABLE(minctoraw minctoraw/minctoraw.c)
ADD_EXECUTABLE(mincwindow mincwindow/mincwindow.c)


ADD_EXECUTABLE(mincmorph mincmorph/mincmorph.c
                         mincmorph/kernel_io.c
                         mincmorph/kernel_ops.c 
                         mincmorph/kernel_io.h 
                         mincmorph/kernel_ops.h )
TARGET_LINK_LIBRARIES(mincmorph ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)


ADD_EXECUTABLE(mincsample mincsample/mincsample.c
                mincsample/mt19937ar.c
                mincsample/mt19937ar.h )
TARGET_LINK_LIBRARIES(mincsample ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(rawtominc rawtominc/rawtominc.c
                            Proglib/convert_origin_to_start.c)
TARGET_LINK_LIBRARIES(rawtominc m)

ADD_EXECUTABLE(voxeltoworld coordinates/voxeltoworld.c)
TARGET_LINK_LIBRARIES(voxeltoworld ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(worldtovoxel coordinates/worldtovoxel.c)
TARGET_LINK_LIBRARIES(worldtovoxel ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(transformtags xfm/transformtags.c)
TARGET_LINK_LIBRARIES(transformtags ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(xfmconcat xfm/xfmconcat.c)
TARGET_LINK_LIBRARIES(xfmconcat ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

ADD_EXECUTABLE(xfminvert xfm/xfminvert.c)
TARGET_LINK_LIBRARIES(xfminvert ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

# install progs
INSTALL(TARGETS
   invert_raw_image 
   mincaverage
   mincconcat
   mincconvert
   minccopy
   mincdump
#   mincexample1
#   mincexample2
   mincexpand
   mincextract
   mincinfo
   minclookup
   mincmakescalar
   mincmakevector
   mincmath
   minc_modify_header
   mincresample
   mincreshape
   mincstats
   minctoraw
   mincwindow
   mincmorph
   mincsample
   rawtominc
   voxeltoworld
   worldtovoxel
   transformtags
   xfmconcat
   xfminvert
   DESTINATION bin)

# perl and shell scripts
INSTALL(PROGRAMS
   mincdiff/mincdiff  
   mincedit/mincedit  
   mincheader/mincheader  
   mincview/mincview
   PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ  WORLD_EXECUTE WORLD_READ
   DESTINATION bin )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/minchistory/minchistory.in ${CMAKE_CURRENT_BINARY_DIR}/minchistory @ONLY)
add_custom_target(minchistory chmod +x ${CMAKE_CURRENT_BINARY_DIR}/minchistory DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/minchistory/minchistory.in)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mincpik/mincpik.in ${CMAKE_CURRENT_BINARY_DIR}/mincpik @ONLY)
add_custom_target(mincpik chmod +x ${CMAKE_CURRENT_BINARY_DIR}/mincpik DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mincpik/mincpik.in)


INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/minchistory
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ  WORLD_EXECUTE WORLD_READ
   DESTINATION bin )

INSTALL(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/mincpik
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ  WORLD_EXECUTE WORLD_READ
   DESTINATION bin )
