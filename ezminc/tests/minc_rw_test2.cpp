#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"
#include "minc_io_simple_volume.h"

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
    rdr.open(argv[1],true);
    rdr.setup_read_float();
    simple_volume<float> vol;
    vol.resize(rdr.ndim(1),rdr.ndim(2),rdr.ndim(3));
    load_non_standard_volume<float>(rdr,vol.c_buf());
    //rdr.close();
    for(int z=0;z<vol.dim(2);z++)
      for(int y=0;y<vol.dim(1);y++)
        for(int x=0;x<vol.dim(0);x++)
        {
          vol(x,y,z)=x;
        }
    minc_1_writer wrt;
    wrt.open(argv[2],rdr.info(),3,NC_FLOAT,false);
    wrt.setup_write_float();
    save_non_standard_volume<float>(wrt,vol.c_buf());
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

