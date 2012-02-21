# CMakeFiles.txt for the MINC2 conversion progs
#
# Andrew Janke - a.janke@gmail.com

#LINK_DIRECTORIES()
IF(BUILD_MINC2)
  LINK_LIBRARIES( minc2 ${NETCDF_LIBRARY} ${HDF5_LIBRARY} )
ELSE(BUILD_MINC2)
  LINK_LIBRARIES( minc ${NETCDF_LIBRARY} )
ENDIF(BUILD_MINC2)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)


# conversion progs (and libraries)
INCLUDE_DIRECTORIES(Acr_nema)
ADD_LIBRARY(acr_nema STATIC 
   Acr_nema/acr_io.c
   Acr_nema/dicom_client_routines.c
   Acr_nema/dicom_network.c
   Acr_nema/element.c
   Acr_nema/file_io.c
   Acr_nema/globals.c
   Acr_nema/group.c
   Acr_nema/message.c
   Acr_nema/value_repr.c)

ADD_EXECUTABLE(dcm2mnc
   dcm2mnc/dcm2mnc.c
   dcm2mnc/dicom_to_minc.c
   dcm2mnc/siemens_to_dicom.c
   dcm2mnc/dicom_read.c
   dcm2mnc/minc_file.c
   dcm2mnc/progress.c
   dcm2mnc/string_to_filename.c)
TARGET_LINK_LIBRARIES(dcm2mnc acr_nema )

ADD_EXECUTABLE(ecattominc
   ecattominc/ecattominc.c
   ecattominc/insertblood.c
   ecattominc/ecat_file.c
   ecattominc/machine_indep.c
   )
ADD_EXECUTABLE(minctoecat
   minctoecat/minctoecat.c
   minctoecat/ecat_write.c
   minctoecat/machine_indep.c
   )

IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(minctoecat volume_io2 minc2)
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(minctoecat volume_io minc)
ENDIF(BUILD_MINC2)


ADD_EXECUTABLE(mnc2nii
   nifti1/mnc2nii.c
   nifti1/nifti1_io.c
   nifti1/znzlib.c
   )
ADD_EXECUTABLE(nii2mnc
   nifti1/nii2mnc.c
   nifti1/nifti1_io.c
   nifti1/znzlib.c
   )

IF(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(nii2mnc volume_io2 minc2)
ELSE(BUILD_MINC2)
  TARGET_LINK_LIBRARIES(nii2mnc volume_io minc)
ENDIF(BUILD_MINC2)

ADD_EXECUTABLE(upet2mnc
   micropet/upet2mnc.c
   )

ADD_EXECUTABLE(vff2mnc
   vff2mnc/vff2mnc.c
   )


# install progs
INSTALL(TARGETS
   dcm2mnc
   ecattominc
   minctoecat
   mnc2nii
   nii2mnc
   upet2mnc
   vff2mnc
   DESTINATION bin)
