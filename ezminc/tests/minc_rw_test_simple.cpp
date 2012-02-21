#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"

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
    
    minc_1_writer wrt;
    for(int i=0;i<3;i++)
    {
      std::cout<<rdr.info()[i].step<<" "<<rdr.info()[i].start<<std::endl;
    }
    wrt.open(argv[2],rdr.info(),3,NC_FLOAT,false);
    wrt.setup_write_float();
    minc_input_iterator<float> in(rdr);
    minc_output_iterator<float> out(wrt);
    float c=0.0;
    for(in.begin(),out.begin();!in.last();in.next(),out.next())
    {
      out.value(in.value()+c);
      c++;
    }
    
	} catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

