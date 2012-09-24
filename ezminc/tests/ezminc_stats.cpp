#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "minc_1_rw.h"
#include "minc_1_simple.h"

using namespace minc;



template<class TPixel> double calc_stats(const char * filename)
{

  //reading volume
  minc_1_reader rdr;
  rdr.open(filename);
  
 
  int volume=1.0;
  for(int i=0;i<3;i++)
  {
    volume*=rdr.info()[i].length;
  }
  
  if(typeid(TPixel)==typeid(unsigned char))
    rdr.setup_read_byte();
  else if(typeid(TPixel)==typeid(int))
    rdr.setup_read_int();
  else if(typeid(TPixel)==typeid(float))
    rdr.setup_read_float(); 
  else if(typeid(TPixel)==typeid(double))
    rdr.setup_read_double();
  else 
    REPORT_ERROR("Data type not supported for minc io");
  
  
  std::vector<TPixel> in_buffer(volume);
  load_standard_volume(rdr,&in_buffer[0]);
  rdr.close();
  
  double mean=0.0;
  
  for(int i=0;i<volume;i++)
    mean+=in_buffer[i];
  
  mean/=volume;
  
  return mean;
}


int main(int argc,char **argv)
{
  try
  {
    if(argc<2) 
    {
      std::cerr<<"Usage:"<<argv[0]<<" input.mnc"<<std::endl;
      return 1;
    }
    std::cout.precision(10);
    std::cout<<"byte mean:"  <<calc_stats<unsigned char>(argv[1])<<std::endl;
    std::cout<<"int mean:"   <<calc_stats<int>(argv[1])<<std::endl;
    std::cout<<"float mean:" <<calc_stats<float>(argv[1])<<std::endl;
    std::cout<<"double mean:"<<calc_stats<double>(argv[1])<<std::endl;
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

