/* ----------------------------- MNI Header -----------------------------------
@NAME       :  volume_similarity
@DESCRIPTION:  an example of calculating volume similarity metrics 
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
  std::cout<<"Program calculates volume similarity metrics"<<std::endl
           <<"if multiple labels are present, similarity metrics will be calculated for each label separately"<<std::endl
          <<"based on : M. Feuerman and A. R. Miller "
          <<"\"Relationships between statistical measures of agreement: sensitivity, specificity and kappa\""
          <<" Journal of Evaluation in Clinical Practice vol. 14 no. 5 pp 930-933 2008"<<std::endl
          <<"http://dx.doi.org/10.1111/j.1365-2753.2008.00984.x"<<std::endl<<std::endl
          <<"Usage: "<<prog<<" <input1.mnc> <input2.mnc> [--kappa --sensitivity --specificity --verbose --jaccard --csv]"<<std::endl;
  
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
  int csv=0;
  int kappa=0,specificity=0,sensitivity=0;
  int jaccard=0;
  static struct option long_options[] = {
    {"verbose", no_argument,             &verbose, 1},
    {"quiet",   no_argument,             &verbose, 0},
    {"kappa",   no_argument,             &kappa, 1},
    {"sensitivity",   no_argument,       &sensitivity, 1},
    {"specificity",   no_argument,       &specificity, 1},
    {"jaccard",       no_argument,       &jaccard, 1},
    {"csv",           no_argument,       &csv, 1},
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
      case 0:
        break;
      case 'v':
        std::cout << "Version: 0.1" << std::endl;
        return 0;
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
  
  if(! (sensitivity||specificity||kappa||jaccard) ) //no options selected, do all 
  {
    verbose=1;
    sensitivity=1;
    specificity=1;
    kappa=1;
    jaccard=1;
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
    
    rdr1.setup_read_byte();
    rdr2.setup_read_byte();
    
    std::vector<unsigned char> buffer1(size),buffer2(size);
    
    load_standard_volume<unsigned char>(rdr1,&buffer1[0]);
    load_standard_volume<unsigned char>(rdr2,&buffer2[0]);
    
    find_min_max<unsigned char> f1,f2;
    
    //get min and max
    f1=std::for_each(buffer1.begin(), buffer1.end(), f1);
    f2=std::for_each(buffer2.begin(), buffer2.end(), f2);
    
    if( verbose && !csv)
    {
      //std::cout<<buffer1.size() << std::endl;
      //std::cout<<buffer1.begin()<<" "<<buffer1.end()<<std::endl;

      std::cout<<"Volume 1 min="<<(int)f1.min()<<" max="<<(int)f1.max()<<" count="<<f1.count()<<std::endl;
      std::cout<<"Volume 2 min="<<(int)f2.min()<<" max="<<(int)f2.max()<<" count="<<f2.count()<<std::endl;
    }

    unsigned char low= std::min<unsigned char>(f1.min(), f2.min());
    unsigned char hi = std::max<unsigned char>(f1.max(), f2.max());

    if(low==0) low=1; //assume 0 is background


    for(unsigned char label=low; label<=hi; label++)
    {
      int v1=0,v2=0;
      double a=0.0,b=0.0,c=0.0,d=0.0;
      for(int i=0; i<size ; i++ )
      {
        if( buffer1[i]==label ) v1++;
        if( buffer2[i]==label ) v2++;
        if( buffer1[i]==label && buffer2[i]==label ) a++;
        if( buffer1[i]==label && buffer2[i]!=label ) c++;
        if( buffer1[i]!=label && buffer2[i]==label ) b++;
        if( buffer1[i]!=label && buffer2[i]!=label ) d++;
      }
      double _kappa=0.0;
      double _sensitivity=0.0;
      double _specificity=0.0;
      double _jaccard=0.0;

      if(v1>0 && v2>0)
      {
        //_kappa=v1v2*2.0/(v1+v2);
        _kappa=a*2.0/(v1+v2);

        _sensitivity=a/(a+c);
        _specificity=d/(b+d);

        _jaccard=a/(a+c+b);
      }

      std::cout.precision(10);

      if( hi!=low && verbose && !csv)
        std::cout<<"Label: "<<(int)label<<std::endl;

      if( csv )
      {
        if(low!=hi)
          std::cout<<(int)label<<",";

        std::cout<<_kappa<<",";
        std::cout<<_sensitivity<<",";
        std::cout<<_specificity<<",";
        std::cout<<_jaccard<<std::endl;
      } else {
        if( kappa ){
          if(verbose) std::cout<<"Kappa ";
          std::cout<<_kappa<<std::endl;
        }
        if( sensitivity )
        {
          if(verbose) std::cout<<"Sensitivity ";
          std::cout<<_sensitivity<<std::endl;
        }
        if( specificity )
        {
          if(verbose) std::cout<<"Specificity ";
          std::cout<<_specificity<<std::endl;
        }
        if( jaccard )
        {
          if(verbose) std::cout<<"Jaccard similarity ";
          std::cout<<_jaccard<<std::endl;
        }
      }
    }
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  return 0;
}
