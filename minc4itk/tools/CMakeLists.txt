LINK_LIBRARIES( minc4itk )


ADD_EXECUTABLE(itk_distance itk_distance.cpp)
ADD_EXECUTABLE(itk_resample itk_resample.cpp)


INSTALL(TARGETS
  itk_distance
  itk_resample
   DESTINATION bin)
