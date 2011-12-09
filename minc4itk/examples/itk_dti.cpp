/* ----------------------------- MNI Header -----------------------------------
@NAME       :  itk_dti
@DESCRIPTION:  an example of processing DTI information
@COPYRIGHT  :
              Copyright 2011 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include <iostream>

#include <itkMetaDataObject.h>
#include <itkVectorImage.h>

#include <itkImage.h>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageIOFactory.h>
#include <itkDiffusionTensor3DReconstructionImageFilter.h>
#include <itkTensorFractionalAnisotropyImageFilter.h>

#include "itkMincImageIOFactory.h"
#include "itkMincImageIO.h"

typedef itk::VectorImage<float, 3>  DTIImageType;
typedef itk::DiffusionTensor3DReconstructionImageFilter<float,float,float> DtiReconstructionFilter;
typedef itk::Image< itk::DiffusionTensor3D< float >, 3 > TensorImage;
typedef DtiReconstructionFilter::GradientDirectionContainerType Gradients;
typedef itk::Image<float,3> faImage;
typedef itk::TensorFractionalAnisotropyImageFilter<TensorImage,faImage > FAFilter;


// a helper function for minc reading
template <class T> typename T::Pointer load_minc(const char *file)
{
  typedef itk::MincImageIO ImageIOType;
  ImageIOType::Pointer minc2ImageIO = ImageIOType::New();
     
  typename itk::ImageFileReader<T>::Pointer reader = itk::ImageFileReader<T>::New();
    
  reader->SetFileName(file);
  reader->SetImageIO( minc2ImageIO );
  reader->Update();
    
  return reader->GetOutput();
}

// a helper function for minc writing
template <class T> void save_minc(const char *file,typename T::ConstPointer img)
{
  typedef itk::MincImageIO ImageIOType;
  ImageIOType::Pointer minc2ImageIO = ImageIOType::New();
     
  typename itk::ImageFileWriter< T >::Pointer writer = itk::ImageFileWriter<T>::New();
  writer->SetFileName(file);
  writer->SetImageIO( minc2ImageIO );
  writer->SetInput( img );
  writer->Update();
}


int main(int argc,char **argv)
{
  if(argc<3)
  {
    std::cerr<<"Usage:"<<argv[0]<<" <in.mnc> <out_tensor.mnc> <out_eign.mnc> "<<std::endl;
    return 1;
  }
  
  try
  {
    std::cout<<"Reading "<<argv[1]<<"..."<<std::endl;
    
    typedef itk::MincImageIO ImageIOType;
    ImageIOType::Pointer minc2ImageIO = ImageIOType::New();
    DTIImageType::Pointer img=load_minc<DTIImageType>(argv[1]);
    DtiReconstructionFilter::Pointer filter=DtiReconstructionFilter::New();
    filter->SetNumberOfThreads(1); //as per request in documentation
    
    //extracting the parameters of acquisition from the metadata
    typedef std::vector<double> double_vector;
    double_vector bvalues,direction_x,direction_y,direction_z;

    //making sure that all vcrtors contain the same number of parameters (just in case)
    if(!itk::ExposeMetaData<double_vector>( img->GetMetaDataDictionary() , "acquisition:bvalues",bvalues) ||
        !itk::ExposeMetaData<double_vector>( img->GetMetaDataDictionary() , "acquisition:direction_x",direction_x) ||
        !itk::ExposeMetaData<double_vector>( img->GetMetaDataDictionary() , "acquisition:direction_y",direction_y) ||
        !itk::ExposeMetaData<double_vector>( img->GetMetaDataDictionary() , "acquisition:direction_z",direction_z))
    {
      std::cerr<<"Image doesn't have information on DTI gradients, can't process!"<<std::endl;
      return 2;
    }
    if(bvalues.size()!=direction_x.size() || 
       bvalues.size()!=direction_y.size() ||
       bvalues.size()!=direction_z.size() )
    {
      std::cerr<<"Different number of components of gradients"<<std::endl;
        return 2;
    }
    std::cout<<"Found "<<bvalues.size()<<" gradient directions"<<std::endl;
    
    //converting metadata representation to the format used by DiffusionTensor3DReconstructionImageFilter
    Gradients::Pointer gradients=Gradients::New();
    gradients->resize(direction_x.size());
    double bval=0;
    //copying values one-by-one
    for(int i=0;i<bvalues.size();i++)
      if(bval<bvalues[i]) bval=bvalues[i];
    
    for(int i=0;i<direction_x.size();i++)
    {
      (*gradients)[i][0]=direction_x[i]*sqrt(bvalues[i]/bval);
      (*gradients)[i][1]=direction_y[i]*sqrt(bvalues[i]/bval);
      (*gradients)[i][2]=direction_z[i]*sqrt(bvalues[i]/bval);
    }
    
    std::cout<<"Calculating tensor..."<<std::endl;
    filter->SetGradientImage(gradients,img);
    filter->SetBValue(bval);
    filter->Update();
    
    std::cout<<"Writing "<<argv[2]<<"..."<<std::endl;
    save_minc<TensorImage>(argv[2],filter->GetOutput() );
    std::cout<<"Calculating FA ..."<<std::endl;
    
    FAFilter::Pointer fa=FAFilter::New();
    fa->SetInput(filter->GetOutput());
    fa->SetNumberOfThreads(1); //just in case
    fa->Update();
    
    std::cout<<"Writing "<<argv[3]<<"..."<<std::endl;
    save_minc<faImage>(argv[3],fa->GetOutput() );
    
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return 2;
  }
  return 0;
}
