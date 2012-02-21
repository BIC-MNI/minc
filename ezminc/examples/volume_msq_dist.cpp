/* ----------------------------- MNI Header -----------------------------------
@NAME       :  volume_msq_dist
@DESCRIPTION:  an example of calculating mean squared dissimilarity
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

using namespace minc;

int main(int argc,char **argv)
{
  
	try
  {
		if(argc<4) {
      std::cerr<<"Usage: "<<argv[0]<<" <input1.mnc> <input2.mnc> <mask.mnc>"<<std::endl;
      return 1;
    }
    minc_1_reader rdr1;
    rdr1.open(argv[1]);
    
    minc_1_reader rdr2;
    rdr2.open(argv[2]);
    
    minc_1_reader rdr_m;
    rdr_m.open(argv[3]);
   
    if(rdr1.dim_no()!=rdr2.dim_no() || rdr1.dim_no()!=rdr_m.dim_no())
    {
      std::cerr<<"Different number of dimensions!"<<std::endl;
      return 1;
    }
    unsigned long size=1;
    for(int i=0;i<5;i++)
    {
      if(rdr1.ndim(i)!=rdr2.ndim(i) || rdr1.ndim(i)!=rdr_m.ndim(i))
      {
        std::cerr<<"Different dimensions length! "<<std::endl;
      }
      if(rdr1.ndim(i)>0) size*=rdr1.ndim(i);
    }
    
    for(int i=0;i<5;i++)
    {
      if(rdr1.nspacing(i)!=rdr2.nspacing(i) ||  rdr1.nspacing(i)!=rdr_m.nspacing(i))
      {
        std::cerr<<"Different step size! "<<std::endl;
      }
    }
    
    //std::cout<<size<<std::endl;
    rdr1.setup_read_float();
    rdr2.setup_read_float();
    rdr_m.setup_read_byte();
    
    std::vector<float> buffer1(size),buffer2(size);
    std::vector<unsigned char> mask(size);
    
    load_standard_volume<float>(rdr1,&buffer1[0]);
    load_standard_volume<float>(rdr2,&buffer2[0]);
    load_standard_volume<unsigned char>(rdr_m,&mask[0]);
    
    double avg=0;
    int cnt=0;
    for(int i=0;i<size;i++)
    {
      if(!mask[i]) continue;
      double d=buffer1[i]-buffer2[i];
      avg+=d*d;
      cnt++;
    }
    if(cnt)
      avg/=cnt;
    if( rdr1.ndim(0)==3 )  //this is a grid file
      avg*=3;
    std::cout.precision(10);
    std::cout<<avg<<std::endl;
    
	} catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

