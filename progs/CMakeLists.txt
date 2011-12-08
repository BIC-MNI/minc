# CMakeFiles.txt for the MINC2 progs
#
# Andrew Janke - a.janke@gmail.com

#SET(CMAKE_MODULE_PATH "cmake-modules")

#LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/..)
INCLUDE_DIRECTORIES(Proglib)

IF(BUILD_MINC2)
  LINK_LIBRARIES( minc2 ${NETCDF_LIBRARY} ${HDF5_LIBRARY} )
ELSE(BUILD_MINC2)
  LINK_LIBRARIES( minc ${NETCDF_LIBRARY} )
ENDIF(BUILD_MINC2)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

#SET(BISON_FIND_REQUIRED)
#SET(FLEX_FIND_REQUIRED)

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

# all the progs
ADD_EXECUTABLE(invert_raw_image mincview/invert_raw_image.c)
ADD_EXECUTABLE(mincaverage mincaverage/mincaverage.c)

IF(BISON_FOUND AND FLEX_FOUND)

  BISON_TARGET(minccalc minccalc/gram.y minccalc/gram.c COMPILE_FLAGS "--debug")
  FLEX_TARGET(minccalc minccalc/lex.l minccalc/lex.c)
  ADD_EXECUTABLE(minccalc minccalc/minccalc.c
                            minccalc/gram.c
                            minccalc/eval.c
                            minccalc/ident.c
                            minccalc/lex.c
                            minccalc/node.c
                            minccalc/optim.c
                            minccalc/scalar.c
                            minccalc/sym.c
                            minccalc/vector.c)
  INSTALL( TARGETS minccalc    DESTINATION bin)

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

IF(BISON_FOUND AND FLEX_FOUND)

  BISON_TARGET(mincgen mincgen/ncgentab.y mincgen/nccentab.c)
  FLEX_TARGET(mincgen mincgen/ncgenyy.l mincgen/ncgenyy.c)
  ADD_EXECUTABLE(mincgen mincgen/main.c
                            mincgen/escapes.c
                            mincgen/genlib.c
                            mincgen/getfill.c
                            mincgen/init.c
                            mincgen/load.c
                            mincgen/ncgentab.c
                            mincgen/ncgenyy.c)
  INSTALL( TARGETS mincgen    DESTINATION bin)

ENDIF(BISON_FOUND AND FLEX_FOUND)

ADD_EXECUTABLE(mincinfo mincinfo/mincinfo.c)
ADD_EXECUTABLE(minclookup minclookup/minclookup.c)
ADD_EXECUTABLE(mincmakescalar mincmakescalar/mincmakescalar.c)
ADD_EXECUTABLE(mincmakevector mincmakevector/mincmakevector.c)
ADD_EXECUTABLE(mincmath mincmath/mincmath.c)
ADD_EXECUTABLE(minc_modify_header minc_modify_header/minc_modify_header.c)

ADD_EXECUTABLE(mincresample mincresample/mincresample.c
                               mincresample/resample_volumes.c
                               Proglib/convert_origin_to_start.c)
TARGET_LINK_LIBRARIES(mincresample volume_io2)

ADD_EXECUTABLE(mincreshape mincreshape/mincreshape.c
                              mincreshape/copy_data.c)

ADD_EXECUTABLE(mincstats mincstats/mincstats.c)
ADD_EXECUTABLE(minctoraw minctoraw/minctoraw.c)
ADD_EXECUTABLE(mincwindow mincwindow/mincwindow.c)

ADD_EXECUTABLE(rawtominc rawtominc/rawtominc.c
                            Proglib/convert_origin_to_start.c)

ADD_EXECUTABLE(voxeltoworld coordinates/voxeltoworld.c)
IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(voxeltoworld volume_io2 )
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(voxeltoworld volume_io )
ENDIF(BUILD_MINC2)

ADD_EXECUTABLE(worldtovoxel coordinates/worldtovoxel.c)
IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(worldtovoxel volume_io2 )
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(worldtovoxel volume_io )
ENDIF(BUILD_MINC2)


ADD_EXECUTABLE(transformtags xfm/transformtags.c)
IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(transformtags volume_io2 )
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(transformtags volume_io )
ENDIF(BUILD_MINC2)

ADD_EXECUTABLE(xfmconcat xfm/xfmconcat.c)
IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(xfmconcat volume_io2 )
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(xfmconcat volume_io )
ENDIF(BUILD_MINC2)

ADD_EXECUTABLE(xfminvert xfm/xfminvert.c)
IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(xfminvert volume_io2 )
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(xfminvert volume_io )
ENDIF(BUILD_MINC2)

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
   rawtominc
   voxeltoworld
   worldtovoxel
   transformtags
   xfmconcat
   xfminvert
   DESTINATION bin)

# perl and shell scripts
INSTALL(FILES
   mincdiff/mincdiff  
   mincedit/mincedit  
   mincheader/mincheader  
   minchistory/minchistory  
   mincpik/mincpik  
   mincview/mincview
   DESTINATION bin )
