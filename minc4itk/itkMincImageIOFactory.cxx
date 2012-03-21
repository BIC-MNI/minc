#include "itkMincImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkMincImageIO.h"
#include <itkVersion.h>
#include <iostream>

#if ( ITK_VERSION_MAJOR > 3 ) 
//TODO:
//#include <itkHDF5ImageIOFactory.h>
#endif //( ITK_VERSION_MAJOR > 3 ) 

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

  static bool MincImageIOFactoryHasBeenRegistered=false;
  
  void RegisterMincIO(void)
  {
    if( ! MincImageIOFactoryHasBeenRegistered )
    {
      MincImageIOFactoryHasBeenRegistered = true;
      MincImageIOFactory::RegisterOneFactory();
    }
    //TODO: deregister HDF IO
  }

#if ( ITK_VERSION_MAJOR > 3 ) 
// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.


  void MincImageIOFactoryRegister__Private(void)
  {
    RegisterMincIO();
  }
  
#endif   //( ITK_VERSION_MAJOR > 3 ) 
} // end namespace itk

