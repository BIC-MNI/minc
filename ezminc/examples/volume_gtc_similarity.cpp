/* ----------------------------- MNI Header -----------------------------------
@NAME       :  volume_gtc_similarity
@DESCRIPTION:  an example of calculating generalized volume similarity metrics 
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
#include <string.h>
#include <map>

using namespace minc;

void show_usage (const char * prog)
{
  
  std::cout<<"Program calculates multiple volume similarity metrics for discrete labels "<<std::endl
           <<"or Generalized Tanimoto coefficient (GTC)" <<std::endl
           <<"based on :  William R. Crum, Oscar Camara, and Derek L. G. Hill"
           <<"\"Generalized Overlap Measures for Evaluation and Validation in Medical Image Analysis \""
           <<" IEEE TRANSACTIONS ON MEDICAL IMAGING, VOL. 25, NO. 11, NOVEMBER 2006"<<std::endl
           <<"http://dx.doi.org/10.1109/TMI.2006.880587"<<std::endl<<std::endl
           <<"Usage: "<<prog<<" <input1.mnc> <input2.mnc> [--gkappa]  [--akappa] [--gtc] [--csv] [--exclude l1[,l2[,l3]]] "<<std::endl;
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
  int gkappa=0;
  int gtc=0;
  int akappa=0;
  
  static struct option long_options[] = {
    {"verbose", no_argument,             &verbose, 1},
    {"quiet",   no_argument,             &verbose, 0},
    {"gkappa",   no_argument,            &gkappa, 1},
    {"akappa",   no_argument,            &akappa, 1},
    {"gtc",      no_argument,            &gtc, 1},
    {"csv",      no_argument,            &csv, 1},
    {"exclude",  required_argument,      0,      'e'},
   
    {0, 0, 0, 0}
  };
  
  std::vector<int> exclude;
  
  for (;;) {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "vqe:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) break;

    switch (c)
    {
      case 0:
        break;
        
      case 'v':
        std::cout << "Version: 0.1" << std::endl;
        return 0;
        
      case 'e':
        {
          const char* delim=", ";
          
          for(char *tok=strtok(optarg,delim);tok;tok=strtok(NULL,delim))
            exclude.push_back(atoi(tok));
        }
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
  
  if(! (gkappa||gtc||akappa) ) //no options selected, do all 
  {
    verbose=1;
    gkappa=1;
    gtc=1;
    akappa=1;
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
      std::cout<<"Volume 1 min="<<(int)f1.min()<<" max="<<(int)f1.max()<<" count="<<f1.count()<<std::endl;
      std::cout<<"Volume 2 min="<<(int)f2.min()<<" max="<<(int)f2.max()<<" count="<<f2.count()<<std::endl;
    }

    unsigned char low= std::min<unsigned char>(f1.min(), f2.min());
    unsigned char hi = std::max<unsigned char>(f1.max(), f2.max());

    if(low==0) low=1; //assume 0 is background

    double intersect=0.0;
    double overlap=0.0;
    double volume=0.0;
    double _akappa=0.0;
    double _akappa_cnt=0.0;
    
    //tools for calculating average kappa ( MICCAI2012 MultiAtlas segmentation style)
    double count_intersect,kappa;
    double count_ref,count_res;

    //TODO: add mask ? or different weighting
    for(unsigned char label=low; label<=hi; label++)
    {
      if( !exclude.empty() && std::find<std::vector<int>::iterator,int>(exclude.begin(),exclude.end(),label)!=exclude.end() )
        continue; //skip this label
        
      count_intersect=0.0;
      count_ref=0.0;
      count_res=0.0;

      for(int i=0; i<size ; i++ )
      {
        if( buffer1[i]==label ) {volume+=1.0;count_ref+=1.0;}
        if( buffer2[i]==label ) {volume+=1.0;count_res+=1.0;}

        if( buffer1[i]==label && buffer2[i]==label ) {intersect+=1.0;count_intersect+=1.0;}
        if( buffer1[i]==label || buffer2[i]==label ) overlap+=1.0;
      }
      if( (count_ref+count_res)>0.0)
      {
        kappa=2.0*count_intersect/(count_ref+count_res);
        _akappa+=kappa;
        _akappa_cnt+=1.0;
      }
    }
    
    double _gkappa=2*intersect/volume;
    double _gtc=intersect/overlap;
    _akappa=_akappa/_akappa_cnt;
    
    std::cout.precision(10);

    if( csv )
    {
      std::cout<<_gkappa<<",";
      std::cout<<_gtc<<",";
      std::cout<<_akappa<<std::endl;
      
    } else {
      if( gkappa ){
        if(verbose) std::cout<<"Generalized Kappa ";
        std::cout<<_gkappa<<std::endl;
      }
      if( gtc )
      {
        if(verbose) std::cout<<"Generalized Tinamoto Coeffecient ";
        std::cout<<_gtc<<std::endl;
      }
      if( akappa )
      {
        if(verbose) std::cout<<"Average kappa ";
        std::cout<<_akappa<<std::endl;
        //std::cout<<"Count:"<<_akappa_cnt<<std::endl;
      }
    }
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  return 0;
}
