#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"
#include "minc_1_simple_rw.h"

using namespace minc;

int main(int argc,char **argv)
{
  try
  {
    if(argc<2) {
      std::cerr<<"Usage: "<<argv[0]<<" <input.mnc> "<<std::endl;
      return 1;
    }
    minc_1_reader rdr;
    rdr.open(argv[1],true);
    simple_volume<float> vol;
    load_simple_volume<float>(rdr,vol);
    //calculate COM
    fixed_vec<3,int> i;
    fixed_vec<3,double> w;
    double total=0;
    double tx=0.0,ty=0.0,tz=0.0;
    
    for(i[2]=0;i[2]<vol.dim(2);i[2]++)
      for(i[1]=0;i[1]<vol.dim(1);i[1]++)
        for(i[0]=0;i[0]<vol.dim(0);i[0]++)
        {
          w=vol.voxel_to_world(i);
          tx+=w[0]*vol.get(i);
          ty+=w[1]*vol.get(i);
          tz+=w[2]*vol.get(i);
          total+=vol.get(i);
        }
    tx/=total;
    ty/=total;
    tz/=total;
    std::cout<<tx<<","<<ty<<","<<tz<<std::endl;
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

