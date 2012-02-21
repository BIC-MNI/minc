#include <iostream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageIOFactory.h>
#include <itkImageIOBase.h>
#include <itkFlipImageFilter.h>

#include "itkMincImageIOFactory.h"
#include "itkMincImageIO.h"

#include <getopt.h>

void show_usage (const char *name)
{
  std::cerr 
      << "Usage: "<<name<<" <input> <output> " << std::endl
      << "--clobber clobber output files"<<std::endl
      << "--verbose be verbose"<<std::endl
      << "--inv-x invert X axis"<<std::endl
      << "--inv-y invert Y axis"<<std::endl
      << "--inv-z invert Z axis"<<std::endl
      << "--center set origin to the center of the image"<<std::endl
;
}
typedef itk::ImageIOBase IOBase;
typedef itk::SmartPointer<IOBase> IOBasePointer;

template<class ImageType> void load_and_save_image(IOBase* base,
                    const char *fname,
                    bool inv_x=false,
                    bool inv_y=false,
                    bool inv_z=false,
                    bool center=false,
                    bool verbose=false)
{
  typename itk::ImageFileReader<ImageType >::Pointer reader = itk::ImageFileReader<ImageType >::New();
  typename itk::FlipImageFilter<ImageType >::Pointer flip=itk::FlipImageFilter<ImageType >::New();
  reader->SetImageIO(base);
  reader->SetFileName(base->GetFileName());
  reader->Update();
  
  typename ImageType::Pointer img=reader->GetOutput();
     
  /* WRITING */
  if(verbose)
    std::cout<<"Writing "<<fname<<"..."<<std::endl;
  

  
  if(inv_x||inv_y||inv_z)
  {
    typename itk::FlipImageFilter<ImageType >::FlipAxesArrayType arr;
    arr[0]=inv_x;
    arr[1]=inv_y;
    arr[2]=inv_z;
    flip->SetFlipAxes(arr);
    flip->SetInput(img);
    flip->Update();
    img=flip->GetOutput();
  }
  
  if(center)//move origin to the center of the image
  {
    typename ImageType::RegionType r=img->GetLargestPossibleRegion();
    std::vector<double> corner[3];
    
    typename ImageType::IndexType idx;
    typename ImageType::PointType c;
    
    idx[0]=r.GetIndex()[0]+r.GetSize()[0]/2.0;
    idx[1]=r.GetIndex()[1]+r.GetSize()[1]/2.0;
    idx[2]=r.GetIndex()[2]+r.GetSize()[2]/2.0;
    
    img->TransformIndexToPhysicalPoint(idx,c);
    
    typename ImageType::PointType org=img->GetOrigin();
    
    org[0]-=c[0];
    org[1]-=c[1];
    org[2]-=c[2];
    
    img->SetOrigin(org);
  }  
  
  typename itk::ImageFileWriter< ImageType >::Pointer writer = itk::ImageFileWriter<ImageType >::New();
  writer->SetFileName(fname);
  writer->SetInput( img );
  writer->Update();
  
}

int main(int argc,char **argv)
{
  int verbose=0;
  int clobber=0;
  int inv_x=0,inv_y=0,inv_z=0,center=0;
  //char *history = time_stamp(argc, argv); //maybe we should free it afterwards

  static struct option long_options[] = { 
    {"verbose", no_argument, &verbose, 1},
    {"quiet", no_argument, &verbose, 0},
    {"clobber", no_argument, &clobber, 1},
    {"inv-x", no_argument,  &inv_x, 1},
    {"inv-y", no_argument,  &inv_y, 1},
    {"inv-z", no_argument,  &inv_z, 1},
    {"center", no_argument, &center, 1},
    {0, 0, 0, 0}
  };
    
  int c;
  for (;;)
  {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
    {
      case 0:
        break;
      case '?':
        /* getopt_long already printed an error message. */
      default:
        show_usage (argv[0]);
        return 1;
    }
  }
  
  if((argc - optind)<2)
  {
    show_usage(argv[0]);
    return 1;
  }
  std::string input=argv[optind];
  std::string output=argv[optind+1];
  
  if (!clobber && !access (output.c_str(), F_OK))
  {
    std::cerr << output.c_str () << " Exists!" << std::endl;
    return 1;
  }
 
  try
  {
    //registering the MINC_IO factory
    itk::ObjectFactoryBase::RegisterFactory(itk::MincImageIOFactory::New());
    /* READING */
    if(verbose) 
      std::cout<<"Reading "<<input.c_str()<<"..."<<std::endl;
    
    //try to figure out what we have got
    IOBasePointer io = itk::ImageIOFactory::CreateImageIO(input.c_str(), itk::ImageIOFactory::ReadMode );
    
    if(!io)
      throw itk::ExceptionObject("Unsupported image file type");
    
    io->SetFileName(input.c_str());
    io->ReadImageInformation();

    size_t nd = io->GetNumberOfDimensions();
    size_t nc = io->GetNumberOfComponents();
    itk::ImageIOBase::IOComponentType  ct = io->GetComponentType();
    std::string ct_s = io->GetComponentTypeAsString(ct);
    
    if(verbose)
    {
      std::cout<<"dimensions:"<<nd<<std::endl
          <<"components:"<<nc<<std::endl
          <<"type:"<<ct_s.c_str()<<std::endl;
    }
    
    if(nd==3 && nc==1)
    {
      if(verbose) std::cout<<"Writing 3D image..."<<std::endl;
        switch(ct)
        {
          case itk::ImageIOBase::UCHAR :
            load_and_save_image<itk::Image<unsigned char, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::CHAR :
            load_and_save_image<itk::Image<char, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::USHORT :
            load_and_save_image<itk::Image<unsigned short, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::SHORT :
            load_and_save_image<itk::Image<short, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::INT :
            load_and_save_image<itk::Image<int, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::UINT:
            load_and_save_image<itk::Image<unsigned int, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::FLOAT :
            load_and_save_image<itk::Image<float, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::DOUBLE:
            load_and_save_image<itk::Image<double, 3> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          default:
            itk::ExceptionObject("Unsupported component type");
        }
    } else if((nd==4 && nc==1)) {
      if(verbose) std::cout<<"Writing 4D image..."<<std::endl;
        switch(ct)
        {
          case itk::ImageIOBase::UCHAR:
            load_and_save_image<itk::Image<unsigned char, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::CHAR:
            load_and_save_image<itk::Image<char, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::USHORT:
            load_and_save_image<itk::Image<unsigned short, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::SHORT:
            load_and_save_image<itk::Image<short, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::INT:
            load_and_save_image<itk::Image<int, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::UINT:
            load_and_save_image<itk::Image<unsigned int, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::FLOAT:
            load_and_save_image<itk::Image<float, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::DOUBLE:
            load_and_save_image<itk::Image<double, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          default:
            itk::ExceptionObject("Unsupported component type");
        }
    } else if((nd==3 && nc>1)) {
        if(verbose) std::cout<<"Writing multicomponent 3D image..."<<std::endl;

        switch(ct)
        {
          case itk::ImageIOBase::UCHAR:
            load_and_save_image<itk::VectorImage<unsigned char, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::CHAR:
            load_and_save_image<itk::VectorImage<char, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::USHORT:
            load_and_save_image<itk::VectorImage<unsigned short, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::SHORT:
            load_and_save_image<itk::VectorImage<short, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break;
          case itk::ImageIOBase::INT:
            load_and_save_image<itk::VectorImage<int, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::UINT:
            load_and_save_image<itk::VectorImage<unsigned int, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::FLOAT:
            load_and_save_image<itk::VectorImage<float, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          case itk::ImageIOBase::DOUBLE:
            load_and_save_image<itk::VectorImage<double, 4> >(io,output.c_str(),inv_x,inv_y,inv_z,center,verbose);
            break; 
          default:
            itk::ExceptionObject("Unsupported component type");
        }
    } else {
        throw itk::ExceptionObject("Unsupported number of dimensions");
    }
    
  }
  
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return 2;
  }
  return 0;
}
