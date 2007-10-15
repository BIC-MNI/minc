# CMakeFiles.txt for the MINC2 progs
#
# Andrew Janke - a.janke@gmail.com


LINK_DIRECTORIES(../)
INCLUDE_DIRECTORIES(Proglib)
LINK_LIBRARIES( minc2 ${NETCDF_LIBRARY} ${HDF5_LIBRARY} )
ADD_DEFINITIONS(-DHAVE_CONFIG_H)


# all the progs
ADD_EXECUTABLE(../mincaverage mincaverage/mincaverage.c)

#ADD_EXECUTABLE(../minccalc minccalc/minccalc.c
#                           minccalc/gram.y
#                           minccalc/eval.c
#                           minccalc/ident.c
#                           minccalc/lex.l
#                           minccalc/node.c
#                           minccalc/optim.c
#                           minccalc/scalar.c
#                           minccalc/sym.c
#                           minccalc/vector.c)


ADD_EXECUTABLE(../mincconcat mincconcat/mincconcat.c)
ADD_EXECUTABLE(../mincconvert mincconvert/mincconvert.c)
ADD_EXECUTABLE(../minccopy minccopy/minccopy.c)

ADD_EXECUTABLE(../mincdump mincdump/mincdump.c
                            mincdump/vardata.c
                            mincdump/dumplib.c)
                            
ADD_EXECUTABLE(../mincexpand mincexpand/mincexpand.c)
ADD_EXECUTABLE(../mincextract mincextract/mincextract.c)

#ADD_EXECUTABLE(../mincgen mincgen/main.c
#                          mincgen/escapes.c
#                          mincgen/genlib.c
#                          mincgen/getfill.c
#                          mincgen/init.c
#                          mincgen/load.c
#                          mincgen/ncgentab.y
#                          mincgen/ncgenyy.l)

ADD_EXECUTABLE(../mincinfo mincinfo/mincinfo.c)
ADD_EXECUTABLE(../minclookup minclookup/minclookup.c)
ADD_EXECUTABLE(../mincmakescalar mincmakescalar/mincmakescalar.c)
ADD_EXECUTABLE(../mincmakevector mincmakevector/mincmakevector.c)
ADD_EXECUTABLE(../mincmath mincmath/mincmath.c)

ADD_EXECUTABLE(../mincresample mincresample/mincresample.c
                               mincresample/resample_volumes.c
                               Proglib/convert_origin_to_start.c)
TARGET_LINK_LIBRARIES(../mincresample volume_io2)

ADD_EXECUTABLE(../mincreshape mincreshape/mincreshape.c
                              mincreshape/copy_data.c)

ADD_EXECUTABLE(../mincstats mincstats/mincstats.c)
ADD_EXECUTABLE(../minctoraw minctoraw/minctoraw.c)
ADD_EXECUTABLE(../mincwindow mincwindow/mincwindow.c)

ADD_EXECUTABLE(../rawtominc rawtominc/rawtominc.c
                            Proglib/convert_origin_to_start.c)

#ADD_EXECUTABLE(../minc minc/minc.c)
#ADD_EXECUTABLE(../minc minc/minc.c)
#ADD_EXECUTABLE(../minc minc/minc.c)
#ADD_EXECUTABLE(../minc minc/minc.c)
#ADD_EXECUTABLE(../minc minc/minc.c)


INSTALL(TARGETS 
        ../mincaverage
        ../mincconcat
        ../minccopy
        DESTINATION bin)


# SET(invert_raw_image_SRCS
#    progs/mincview/invert_raw_image.c
#    )
# TARGET_LINK_LIBRARIES(invert_raw_image  ${QT_AND_KDECORE_LIBS} )
# 
# 
# 
# SET(transformtags_SRCS
#    progs/xfm/transformtags.c
#    )
# TARGET_LINK_LIBRARIES(transformtags  ${QT_AND_KDECORE_LIBS} )
# 

# SET(xfmconcat_SRCS
#    progs/xfm/xfmconcat.c
#    )
# TARGET_LINK_LIBRARIES(xfmconcat  ${QT_AND_KDECORE_LIBS} )
# 
# 
# 
# SET(xfminvert_SRCS
#    progs/xfm/xfminvert.c
#    )
# 
# TARGET_LINK_LIBRARIES(xfminvert  ${QT_AND_KDECORE_LIBS} )
# 
# 
# 
# SET(mincdump_SRCS
#    progs/mincdump/mincdump.c
#    progs/mincdump/vardata.c
#    progs/mincdump/dumplib.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincdump  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mincdump DESTINATION bin)
# 
# 
# 






# 
# 
# SET(acr_nema_STAT_SRCS
#    conversion/Acr_nema/acr_io.c
#    conversion/Acr_nema/dicom_client_routines.c
#    conversion/Acr_nema/dicom_network.c
#    conversion/Acr_nema/element.c
#    conversion/Acr_nema/file_io.c
#    conversion/Acr_nema/globals.c
#    conversion/Acr_nema/group.c
#    conversion/Acr_nema/message.c
#    conversion/Acr_nema/value_repr.c
#    )
# 
# ADD_LIBRARY(acr_nema STATIC ${acr_nema_STAT_SRCS})
# 
# 
# 
# SET(voxeltoworld_SRCS
#    progs/coordinates/voxeltoworld.c
#    )
# 
# TARGET_LINK_LIBRARIES(voxeltoworld )
# 
# INSTALL( TARGETS voxeltoworld DESTINATION bin)
# 
# 
# 
# SET(worldtovoxel_SRCS
#    progs/coordinates/worldtovoxel.c
#    )
# 
# TARGET_LINK_LIBRARIES(worldtovoxel )
# 
# INSTALL( TARGETS worldtovoxel DESTINATION bin)
# 
# 
# 
# SET(minc_modify_header_SRCS
#    progs/minc_modify_header/minc_modify_header.c
#    )
# 
# TARGET_LINK_LIBRARIES(minc_modify_header )
# 
# INSTALL( TARGETS minc_modify_header DESTINATION bin)
# 
# 
# 
# SET(ecattominc_SRCS
#    conversion/ecattominc/ecattominc.c
#    conversion/ecattominc/insertblood.c
#    conversion/ecattominc/ecat_file.c
#    conversion/ecattominc/machine_indep.c
#    )
# 
# TARGET_LINK_LIBRARIES(ecattominc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS ecattominc DESTINATION bin)
# 
# 
# 
# SET(minctoecat_SRCS
#    conversion/minctoecat/minctoecat.c
#    conversion/minctoecat/ecat_write.c
#    conversion/minctoecat/machine_indep.c
#    )
# 
# TARGET_LINK_LIBRARIES(minctoecat  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS minctoecat DESTINATION bin)
# 
# 
# 
# SET(mnc2nii_SRCS
#    conversion/nifti1/mnc2nii.c
#    conversion/nifti1/nifti1_io.c
#    conversion/nifti1/znzlib.c
#    )
# 
# TARGET_LINK_LIBRARIES(mnc2nii  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS mnc2nii DESTINATION bin)
# 
# 
# 
# SET(nii2mnc_SRCS
#    conversion/nifti1/nii2mnc.c
#    conversion/nifti1/nifti1_io.c
#    conversion/nifti1/znzlib.c
#    )
# 
# TARGET_LINK_LIBRARIES(nii2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS nii2mnc DESTINATION bin)
# 
# 
# 
# SET(upet2mnc_SRCS
#    conversion/micropet/upet2mnc.c
#    )
# 
# TARGET_LINK_LIBRARIES(upet2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS upet2mnc DESTINATION bin)
# 
# 
# 
# SET(dcm2mnc_SRCS
#    conversion/dcm2mnc/dcm2mnc.c
#    conversion/dcm2mnc/dicom_to_minc.c
#    conversion/dcm2mnc/siemens_to_dicom.c
#    conversion/dcm2mnc/dicom_read.c
#    conversion/dcm2mnc/minc_file.c
#    conversion/dcm2mnc/progress.c
#    conversion/dcm2mnc/string_to_filename.c
#    )
# 
# TARGET_LINK_LIBRARIES(dcm2mnc  ${QT_AND_KDECORE_LIBS} acr_nema )
# 
# INSTALL( TARGETS dcm2mnc DESTINATION bin)
# 
# 
# 
# SET(vff2mnc_SRCS
#    conversion/vff2mnc/vff2mnc.c
#    )
# 
# TARGET_LINK_LIBRARIES(vff2mnc  ${QT_AND_KDECORE_LIBS} )
# 
# INSTALL( TARGETS vff2mnc DESTINATION bin)
# 
# 
# 
# SET(mincexample1_SRCS
#    progs/mincexample/mincexample1.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincexample1  ${QT_AND_KDECORE_LIBS} )
# 
# 
# 
# SET(mincexample2_SRCS
#    progs/mincexample/mincexample2.c
#    )
# 
# TARGET_LINK_LIBRARIES(mincexample2  ${QT_AND_KDECORE_LIBS} )
# 
# 
# ########### install files ###############
# 
# INSTALL(FILES   DESTINATION /include/volume_io )
# INSTALL(FILES   progs/xfm/xfmflip DESTINATION  )
# INSTALL(FILES   progs/mincdiff/mincdiff  
#                 progs/mincedit/mincedit  
#                 progs/mincheader/mincheader  
#                 progs/minchistory/minchistory  
#                 progs/mincpik/mincpik  
#                 progs/mincview/mincview DESTINATION  )
# INSTALL(FILES   DESTINATION /include/acr_nema )
