#include "minc_1_simple_rw.h"
#include <iostream>

using namespace minc;

int main(int argc,char **argv)
{
  try
  {
    if(argc<3) {
      std::cerr<<"Usage: "<<argv[0]<<" <input.mnc> <output.mnc>"<<std::endl;
      return 1;
    }
    minc_1_reader rdr;
    rdr.open(argv[1]);
    
    if(rdr.dim_no()==3|| (rdr.dim_no()==4 && rdr.ndim(4)>0 ))
    {
      if(rdr.datatype()==NC_FLOAT || rdr.datatype()==NC_SHORT)
      {
        std::cout<<"Reading float volume"<<std::endl;
        simple_4d_volume<float> vol;
        
        load_4d_volume(rdr,vol);
        save_minc_file(argv[2],vol,"test",&rdr,rdr.datatype(),rdr.is_signed());
      } else if(rdr.datatype()==NC_BYTE) {
        std::cout<<"Reading byte volume"<<std::endl;
        simple_4d_volume<unsigned char> vol;
        
        load_4d_volume(rdr,vol);
        save_minc_file(argv[2],vol,"test",&rdr,rdr.datatype(),rdr.is_signed());
      }
    } else if((rdr.dim_no()==4|| rdr.dim_no()==5)&&rdr.ndim(0)==3) { //we are dealing with vectors
        std::cout<<"Reading vector volume"<<std::endl;
        simple_4d_volume< fixed_vec<3,float> > vol;
        
        load_4d_volume(rdr,vol);
        save_minc_file(argv[2],vol,"test",&rdr,rdr.datatype(),rdr.is_signed());
    }
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}



