/* ----------------------------- MNI Header -----------------------------------
@NAME       :  fuzzy_volume_similarity
@DESCRIPTION:  an example of implimentation of fuzzy volume similarity 
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
#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"
#include <getopt.h>
#include <algorithm>

using namespace minc;

void show_usage (const char * prog)
{
  std::cout<<"Program calculates fuzzy volume similarity metrics"<<std::endl
          <<"based on :  William R. Crum, Oscar Camara, and Derek L. G. Hill"
          <<"\"Generalized Overlap Measures for Evaluation and Validation in Medical Image Analysis \""
          <<" IEEE TRANSACTIONS ON MEDICAL IMAGING, VOL. 25, NO. 11, NOVEMBER 2006"<<std::endl
          <<"http://dx.doi.org/10.1109/TMI.2006.880587"<<std::endl<<std::endl
          <<"Usage: "<<prog<<" <input1.mnc> <input2.mnc> [--verbose --mask <mask.mnc>]"<<std::endl;
  
}

template<class T>class find_min_max
{
  public:
  T _min;
  T _max;
  bool _initialized;
  int _count;
  
  find_min_max():_initialized(false),_min(0),_max(0),_count(0)
  {
  }
  
  void operator()(const T& v)
  {
    if(_initialized)
    {
      if(v<_min) _min=v;
      if(v>_max) _max=v;
    } else {
      _initialized=true;
      _min=v;
      _max=v;
    }
    _count++;
  }
  
  const T& min(void) const {
    return _min;
  }
  
  const T& max(void) const{
    return _max;
  }
  
  int count(void) const {
    return _count;
  }
};

int main(int argc,char **argv)
{
  int verbose=0;
  std::string mask_f;
  static struct option long_options[] = {
    {"verbose", no_argument,             &verbose, 1},
    {"quiet",   no_argument,             &verbose, 0},
    {"mask",     required_argument,              0,'m'},
    
    {0, 0, 0, 0}
  };
  
  for (;;) {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "vqm:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) break;

    switch (c)
    {
      case 0:
        break;
      case 'v':
        std::cout << "Version: 0.1" << std::endl;
        return 0;
      case 'm':
        mask_f=optarg;
        break;
      case '?':
        /* getopt_long already printed an error message. */
      default:
        show_usage (argv[0]);
        return 1;
    }
  }

  if((argc - optind) < 2) {
    show_usage (argv[0]);
    return 1;
  }
  
  try
  {
    minc_1_reader rdr1;
    rdr1.open(argv[optind]);
    
    minc_1_reader rdr2;
    rdr2.open(argv[optind+1]);
    
    if(rdr1.dim_no()!=rdr2.dim_no() )
    {
      std::cerr<<"Different number of dimensions!"<<std::endl;
      return 1;
    }
    unsigned long size=1;
    
    for(int i=0;i<5;i++)
    {
      if(rdr1.ndim(i)!=rdr2.ndim(i))
        std::cerr<<"Different dimensions length! "<<std::endl;
      
      if(rdr1.ndim(i)>0) size*=rdr1.ndim(i);
    }
    
    for(int i=0;i<5;i++)
    {
      if(rdr1.nspacing(i)!=rdr2.nspacing(i) )
        std::cerr<<"Different step size! "<<std::endl;
    }
    
    std::vector<unsigned char> mask(size,1);
    if(!mask_f.empty())
    {
      minc_1_reader rdr_m;
      rdr_m.open(mask_f.c_str());
      
      for(int i=0;i<5;i++)
      {
        if(rdr1.ndim(i)!=rdr_m.ndim(i))
          std::cerr<<"Different mask dimensions length! "<<std::endl;
        
        if(rdr1.nspacing(i)!=rdr_m.nspacing(i) )
          std::cerr<<"Different mask step size! "<<std::endl;
      }
      rdr_m.setup_read_byte();
      load_standard_volume<unsigned char>(rdr_m,&mask[0]);
    }
    
    rdr1.setup_read_double();
    rdr2.setup_read_double();
    
    std::vector<double> buffer1(size),buffer2(size);
    
    load_standard_volume<double>(rdr1,&buffer1[0]);
    load_standard_volume<double>(rdr2,&buffer2[0]);
    
    find_min_max<double> f1,f2;
    
    //get min and max
    f1=std::for_each(buffer1.begin(), buffer1.end(), f1);
    f2=std::for_each(buffer2.begin(), buffer2.end(), f2);
    
    if( verbose )
    {
      std::cout<<"Volume 1 min="<<(int)f1.min()<<" max="<<(int)f1.max()<<" count="<<f1.count()<<std::endl;
      std::cout<<"Volume 2 min="<<(int)f2.min()<<" max="<<(int)f2.max()<<" count="<<f2.count()<<std::endl;
    }

    unsigned char low= std::min<double>(f1.min(), f2.min());
    unsigned char hi = std::max<double>(f1.max(), f2.max());

    int v1=0,v2=0;
    double a=0.0,b=0.0,c=0.0,d=0.0;
    for(int i=0; i<size ; i++ )
    {
      
      if(mask[i]>0)
      {
        a+=std::min<double>(buffer1[i],buffer2[i]);
        b+=std::max<double>(buffer1[i],buffer2[i]);
      }
    }

    std::cout.precision(10);
    std::cout<<a/b<<std::endl;

    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  return 0;
}
