#include "itkMincImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkMincImageIO.h"
#include <itkVersion.h>
#include <iostream>
namespace itk
{
  void MincImageIOFactory::PrintSelf(std::ostream&, Indent) const
  {
    
  }
  
  
  MincImageIOFactory::MincImageIOFactory()
  {
    this->RegisterOverride("itkImageIOBase",
                          "itkMincImageIO",
                          "Minc Image IO",
                          1,
                          CreateObjectFunction<MincImageIO>::New());
  }
  
  MincImageIOFactory::~MincImageIOFactory()
  {
  }
  
  const char*
  MincImageIOFactory::GetITKSourceVersion(void) const
  {
    return ITK_SOURCE_VERSION;
  }
  
  const char*
  MincImageIOFactory::GetDescription() const
  {
    return "Minc ImageIO Factory, allows the loading of Minc images into insight";
  }

} // end namespace itk

