LINK_LIBRARIES( minc4itk )


ADD_EXECUTABLE(itk_convert itk_convert.cpp)
ADD_EXECUTABLE(itk_distance itk_distance.cpp)
ADD_EXECUTABLE(itk_dti itk_dti.cpp)
ADD_EXECUTABLE(itk_resample itk_resample.cpp)
ADD_EXECUTABLE(volume_2_csv volume_2_csv.cpp)


INSTALL(TARGETS
  itk_convert
  itk_distance
  itk_resample
   DESTINATION bin)
