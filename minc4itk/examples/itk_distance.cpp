/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@DESCRIPTION: 
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <itkDanielssonDistanceMapImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <time_stamp.h>    // for creating minc style history entry
#include "itkMincImageIOFactory.h"
#include "itkMincImageIO.h"
#include "minc_helpers.h"

//#include <minc_wrappers.h>

using namespace minc;
using namespace std;
void show_usage(const char *name)
{
  std::cerr 
    << "Usage: "<<name<<" <input> <output> " << endl
    << "--verbose be verbose "    << endl
    << "--clobber clobber output files"<<endl
    << "--signed produce signed distance map"<<endl;
}

int main (int argc, char **argv)
{
  
  int verbose=1;
  double sigma=0.5;
  double keep=1.0;
  int order=5;
  int approx=0;
  int ss=0;
  int clobber=0;
  char *history = time_stamp(argc, argv); 

  //int voxel_neibourhood=5;
  static struct option long_options[] = { 
    {"clobber", no_argument, &clobber, 1},
    {"verbose", no_argument, &verbose, 1},
    {"quiet", no_argument, &verbose, 0},
    {"signed",no_argument, &ss, 1},
    {0, 0, 0, 0}
  };
  
  for (;;) {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "vq", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) break;

    switch (c)
    {
    //  case 'n':
    //    voxel_neibourhood=atoi(optarg);break;
      case 0:
        break;
      case '?':
        /* getopt_long already printed an error message. */
      default:
        show_usage (argv[0]);
        return 1;
    }
  }
  if ((argc - optind) < 2) {
    show_usage (argv[0]);
    return 1;
  }
  std::string input_f=argv[optind],  out_f=argv[optind+1];
  
  // check if the file already present
  if (!clobber && !access (out_f.c_str (), F_OK))
  {
    cerr << out_f.c_str () << " Exists!" << endl;
    return 1;
  }
    
  try
  {
    itk::ObjectFactoryBase::RegisterFactory(itk::MincImageIOFactory::New());
    itk::ImageFileReader<minc::mask3d >::Pointer reader = itk::ImageFileReader<minc::mask3d >::New();
    
    //initializing the reader
    reader->SetFileName(input_f.c_str());
    reader->Update();
    
    minc::mask3d::Pointer input=reader->GetOutput();
    minc::image3d::Pointer output;
    
    typedef itk::DanielssonDistanceMapImageFilter< minc::mask3d, minc::image3d >
            DistanceMapFilter;
    typedef itk::SignedDanielssonDistanceMapImageFilter< minc::mask3d, minc::image3d >
        SignedDistanceMapFilter;
    
    if(ss)
    {
      SignedDistanceMapFilter::Pointer dist(SignedDistanceMapFilter::New());
      dist->SetInput(input);
      dist->Update();
      output=dist->GetOutput();
    } else {
      DistanceMapFilter::Pointer dist(DistanceMapFilter::New());
      dist->SetInput(input);
      dist->Update();
      output=dist->GetOutput();
    }
    
    minc::copy_metadata(output,input);
    minc::append_history(output,history);
    free(history);
    
    itk::ImageFileWriter< minc::image3d >::Pointer writer = itk::ImageFileWriter<minc::image3d >::New();
    writer->SetFileName(out_f.c_str());
    writer->SetInput( output );
    writer->Update();
    
  } catch (const minc::generic_error & err) {
    cerr << "Got an error at:" << err.file () << ":" << err.line () << endl;
    cerr << err.msg()<<endl;
    return 1;
  }
  return 0;
}
