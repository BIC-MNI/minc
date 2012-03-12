ADD_EXECUTABLE(itk_convert itk_convert.cpp)
TARGET_LINK_LIBRARIES(itk_convert minc4itk)

INSTALL(TARGETS
  itk_convert
   DESTINATION bin)
