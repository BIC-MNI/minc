#include "itkMincImageIOFactory.h"

#ifdef WIN32
#define MyPlugin_EXPORT __declspec(dllexport)
#else
#define MyPlugin_EXPORT 
#endif

extern "C" {
  MyPlugin_EXPORT itk::ObjectFactoryBase* itkLoad();
} 

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */

itk::ObjectFactoryBase* itkLoad()
{
  static itk::MincImageIOFactory::Pointer f=itk::MincImageIOFactory::New();
  return f;
}

