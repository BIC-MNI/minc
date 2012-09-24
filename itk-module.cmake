set(DOCUMENTATION "This module Implements minc IO file format ...")
 
itk_module(
    ITKIOMINC
  DEPENDS
    ITKHDF5
    ITKZLIB
    ITKCommon
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
 
#