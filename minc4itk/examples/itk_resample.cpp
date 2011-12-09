/* ----------------------------- MNI Header -----------------------------------
@NAME       :  itk_resample
@DESCRIPTION:  an example of using spline itnerpolation with MINC xfm transform
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
#include <fstream>
#include <getopt.h>
#include <vector>
#include <valarray>
#include <math.h>
#include <limits>
#include <unistd.h>

#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <minc_general_transform.h>

#include <unistd.h>
#include <getopt.h>
#include <time_stamp.h>    // for creating minc style history entry


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageIOFactory.h>

#include "itkMincImageIOFactory.h"
#include "itkMincImageIO.h"
#include "minc_helpers.h"

//typedef itk::MincImageIO ImageIOType;
typedef itk::BSplineInterpolateImageFunction< minc::image3d, double, double >  InterpolatorType;
typedef itk::ResampleImageFilter<minc::image3d, minc::image3d> FilterType;
typedef minc::XfmTransform<double,3,3>  TransformType;

using namespace  std;

void show_usage (const char * prog)
{
  std::cerr 
    << "Usage: "<<prog<<" <input> <output.mnc> " << std::endl
    << "--clobber overwrite files"    << std::endl
    << "--like <example> (default behaviour analogous to use_input_sampling)"<<std::endl
    << "--transform <xfm_transform> "<<std::endl
    << "--order <n> spline order, default 2 "<<std::endl
    << "--uniformize <step> - will make a volume with uniform step size and no direction cosines" << std::endl
    << "--invert_transform  - apply inverted transform"<<std::endl;
}

void generate_uniform_sampling(FilterType* flt, const minc::image3d* img,double step)
{
  //obtain physical coordinats of all image corners
  minc::image3d::RegionType r=img->GetLargestPossibleRegion();
  std::vector<double> corner[3];
  for(int i=0;i<8;i++)
  {
    minc::image3d::IndexType idx;
    minc::image3d::PointType c;
    idx[0]=r.GetIndex()[0]+r.GetSize()[0]*(i%2);
    idx[1]=r.GetIndex()[1]+r.GetSize()[1]*((i/2)%2);
    idx[2]=r.GetIndex()[2]+r.GetSize()[2]*((i/4)%2);
    img->TransformIndexToPhysicalPoint(idx,c);
    for(int j=0;j<3;j++)
      corner[j].push_back(c[j]);
  }
  minc::image3d::IndexType start;
  FilterType::SizeType size;
  FilterType::OriginPointType org;
  minc::image3d::SpacingType spc;
  spc.Fill(step);
  for(int j=0;j<3;j++)
  {
    std::sort(corner[j].begin(),corner[j].end());
    size[j]=ceil((corner[j][7]-corner[j][0])/step);
    org[j]=corner[j][0];
  }
  minc::image3d::DirectionType identity;
  identity.SetIdentity();
  flt->SetOutputDirection(identity);
  start.Fill(0);
  flt->SetOutputStartIndex(start);
  flt->SetSize(size);
  flt->SetOutputOrigin(org);
  flt->SetOutputSpacing(spc);
}

int main (int argc, char **argv)
{
  int verbose=0, clobber=0,skip_grid=0;
  int order=2;
  std::string like_f,xfm_f,output_f,input_f;
  double uniformize=0.0;
  int invert=0;
  char *history = time_stamp(argc, argv); 
  
  static struct option long_options[] = {
		{"verbose", no_argument,       &verbose, 1},
		{"quiet",   no_argument,       &verbose, 0},
		{"clobber", no_argument,       &clobber, 1},
		{"like",    required_argument, 0, 'l'},
		{"transform",    required_argument, 0, 't'},
    {"order",    required_argument, 0, 'o'},
    {"uniformize",    required_argument, 0, 'u'},
    {"invert_transform", no_argument, &invert, 1},
		{0, 0, 0, 0}
		};
  
  for (;;) {
      /* getopt_long stores the option index here. */
      int option_index = 0;

      int c = getopt_long (argc, argv, "vqcl:t:o:u:", long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1) break;

      switch (c)
			{
			case 0:
				break;
			case 'v':
				cout << "Version: 0.1" << endl;
				return 0;
      case 'l':
        like_f=optarg; break;
      case 't':
        xfm_f=optarg; break;
      case 'o':
        order=atoi(optarg);break;
      case 'u':
        uniformize=atof(optarg);break;
			case '?':
				/* getopt_long already printed an error message. */
			default:
				show_usage (argv[0]);
				return 1;
			}
    }

	if ((argc - optind) < 2) {
		show_usage(argv[0]);
		return 1;
	}
  input_f=argv[optind];
  output_f=argv[optind+1];
  
  if (!clobber && !access (output_f.c_str (), F_OK))
  {
    std::cerr << output_f.c_str () << " Exists!" << std::endl;
    return 1;
  }
  
	try
  {
    itk::ObjectFactoryBase::RegisterFactory(itk::MincImageIOFactory::New());
    itk::ImageFileReader<minc::image3d >::Pointer reader = itk::ImageFileReader<minc::image3d >::New();
    
    //initializing the reader
    reader->SetFileName(input_f.c_str());
    reader->Update();
    
		minc::image3d::Pointer in=reader->GetOutput();

		FilterType::Pointer filter  = FilterType::New();
    
    //creating coordinate transformation objects
		TransformType::Pointer transform = TransformType::New();
    if(!xfm_f.empty())
    {
      //reading a minc style xfm file
      transform->OpenXfm(xfm_f.c_str());
      if(!invert) transform->Invert(); //should be inverted by default to walk through target space
      filter->SetTransform( transform );
    }

    //creating the interpolator
		InterpolatorType::Pointer interpolator = InterpolatorType::New();
		interpolator->SetSplineOrder(order);
		filter->SetInterpolator( interpolator );
		filter->SetDefaultPixelValue( 0 );
    
    //this is for processing using batch system
    filter->SetNumberOfThreads(1);
    
    minc::image3d::Pointer like=0;
    if(!like_f.empty())
    {
      itk::ImageFileReader<minc::image3d >::Pointer reader = itk::ImageFileReader<minc::image3d >::New();
      reader->SetFileName(like_f.c_str());
      reader->Update();
      if(uniformize!=0.0)
      {
        generate_uniform_sampling(filter,reader->GetOutput(),uniformize);
      } else {
        filter->SetOutputParametersFromImage(reader->GetOutput());
        filter->SetOutputDirection(reader->GetOutput()->GetDirection());
      }
      like=reader->GetOutput();
      like->DisconnectPipeline();
    }
    else
    {
      if(uniformize!=0.0)
      {
        generate_uniform_sampling(filter,in,uniformize);
      } else {
        //we are using original sampling
        filter->SetOutputParametersFromImage(in);
        filter->SetOutputDirection(in->GetDirection());
      }
    }
    
		filter->SetInput(in);
    filter->Update();
    //copy the metadate information, for some reason it is not preserved
    //filter->GetOutput()->SetMetaDataDictionary(reader->GetOutput()->GetMetaDataDictionary());
    minc::image3d::Pointer out=filter->GetOutput();
    minc::copy_metadata(out,in);
    minc::append_history(out,history);
    free(history);
    
    //correct dimension order
    if(like.IsNotNull())
      minc::copy_dimorder(out,like);
    
    //generic file writer
    itk::ImageFileWriter< minc::image3d >::Pointer writer = itk::ImageFileWriter<minc::image3d >::New();
    writer->SetFileName(output_f.c_str());
     
    writer->SetInput( out );
    //writer->UseInputMetaDataDictionaryOn();
    
    writer->Update();
    
		return 0;
	} catch (const minc::generic_error & err) {
    cerr << "Got an error at:" << err.file () << ":" << err.line () << endl;
    return 1;
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return 2;
  }
	return 0;
};
