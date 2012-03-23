#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "minc_1_rw.h"
#include "minc_1_simple.h"

using namespace minc;



template<class TPixel> void make_rw_test(const char * filename,int slice_dim,nc_type datatype,bool is_signed,double max_diff=0.0)
{

  std::string history="History ";
  
  // generate info
  minc_info info(3);
  int volume=1.0;
  
  for(int i=0;i<3;i++)
  {
    info[i].dim=dim_info::dimensions( dim_info::DIM_X+i);
    
    info[i].length=10+i;
    info[i].step  =i+0.1;
    info[i].start =i-5.0;
    
    info[i].have_dir_cos=true;
    
    for(int j=0;j<3;j++)
      info[i].dir_cos[j]=(i==j?1.0:0.0);    
    
    volume*=info[i].length;
  }
  
  //fill the buffer
  std::vector<TPixel> buffer(volume);
  
  if(typeid(TPixel)==typeid(float) || typeid(TPixel)==typeid(double))
    for(int i=0;i<volume;i++)
      buffer[i]=static_cast<TPixel>(random())*100.0/RAND_MAX;
  else
    for(int i=0;i<volume;i++)
      buffer[i]=static_cast<TPixel>(random());
  
    
  std::vector<double> double_attr(10);
  std::vector<int> int_attr(10);
  std::vector<short> short_attr(10);
  std::string  string_attr="Test string attribuite";
  
  for(int i=0;i<10;i++)
  {
    double_attr[i]=i+0.1;
    int_attr[i]=i*100;
    short_attr[i]=i;
  }
    
  //now let's write volume
  minc_1_writer wrt;
  wrt.open(filename,info,slice_dim,datatype,is_signed);
  
  //atributes
  wrt.append_history(history.c_str());
  wrt.insert("patient","double_attr",double_attr);
  wrt.insert("patient","int_attr",int_attr);
  wrt.insert("patient","short_attr",short_attr);
  wrt.insert("patient","string_attr",string_attr.c_str());
  
  if(typeid(TPixel)==typeid(unsigned char))
    wrt.setup_write_byte();
  else if(typeid(TPixel)==typeid(int))
    wrt.setup_write_int();
  else if(typeid(TPixel)==typeid(float))
    wrt.setup_write_float(); 
  else if(typeid(TPixel)==typeid(double))
    wrt.setup_write_double(); 
  else 
    REPORT_ERROR("Data type not supported for minc io"); 
  
  
  
  //write volume
  save_standard_volume(wrt,&buffer[0]);
  
  wrt.close();

  //reading volume
  minc_1_reader rdr;
  rdr.open(filename);
  
  if(history!=rdr.history())
      REPORT_ERROR("Mismatched history");
 
  std::vector<double> double_attr_rd=rdr.att_value_double("patient","double_attr");
  std::vector<int>  int_attr_rd=rdr.att_value_int("patient","int_attr");
  std::vector<short> short_attr_rd=rdr.att_value_short("patient","short_attr");
  std::string string_attr_rd=rdr.att_value_string("patient","string_attr");
  
  if(string_attr_rd!=string_attr)
    REPORT_ERROR("Mismatched string attribute");
  
  for(int i=0;i<10;i++)
  {
    if(double_attr_rd[i]!=double_attr[i])
      REPORT_ERROR("Mismatched double attribute");
    if(int_attr_rd[i]!=int_attr[i])
      REPORT_ERROR("Mismatched int attribute");
    if(short_attr_rd[i]!=short_attr[i])
      REPORT_ERROR("Mismatched short attribute");
  }
  // let's compare info
  for(int i=0;i<3;i++)
  {
    if(rdr.info()[i].dim!=info[i].dim) 
      REPORT_ERROR("Mismatched dimension");
    
    if(rdr.info()[i].length!=info[i].length) 
      REPORT_ERROR("Mismatched dimension length");
    
    if(rdr.info()[i].step!=info[i].step) 
      REPORT_ERROR("Mismatched step");
    
    if(rdr.info()[i].start!=info[i].start) 
      REPORT_ERROR("Mismatched step");
    
    if(rdr.info()[i].have_dir_cos!=info[i].have_dir_cos) 
      REPORT_ERROR("Mismatched have direction cosines");
    
    for(int j=0;j<3;j++)
      if(rdr.info()[i].dir_cos[j]!=info[i].dir_cos[j])
        REPORT_ERROR("Mismatched direction cosines");
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
  
  if(max_diff==0.0)
  {
    for(int i=0;i<volume;i++)
      if(buffer[i]!=in_buffer[i])
        REPORT_ERROR("Data mismatched!");
  } else { //ALLOW rounding error
    for(int i=0;i<volume;i++)
      if(fabs(buffer[i]-in_buffer[i])>max_diff)
      {
        std::cerr<<"Expected:"<<buffer[i]<<" got:"<<in_buffer[i]<<" @ "<<i<<std::endl;
        REPORT_ERROR("Data mismatched too much!");
      }
  }
}


int main(int argc,char **argv)
{
  try
  {
    if(argc>1) 
    {
      //chdir(argv[1]);
    }
    
    //no rounding expected
    make_rw_test<unsigned char>("EZminc_byte_2.mnc",2,NC_BYTE,false);
    make_rw_test<unsigned char>("EZminc_byte_3.mnc",3,NC_BYTE,false);
    
    make_rw_test<int>("EZminc_int_2.mnc",2,NC_INT,true);
    make_rw_test<int>("EZminc_int_3.mnc",3,NC_INT,true);
    
    make_rw_test<float>("EZminc_float_2.mnc",2,NC_FLOAT,true);
    make_rw_test<float>("EZminc_float_3.mnc",3,NC_FLOAT,true);
    
    make_rw_test<double>("EZminc_double_2.mnc",2,NC_DOUBLE,true);
    make_rw_test<double>("EZminc_double_3.mnc",3,NC_DOUBLE,true);
    
    // some rounding expected
    make_rw_test<float>("EZminc_float_2_short.mnc",2,NC_SHORT,true,0.1);
    make_rw_test<float>("EZminc_float_3_short.mnc",3,NC_SHORT,true,0.1);
    
    make_rw_test<double>("EZminc_double_2_short.mnc",2,NC_SHORT,true,0.1);
    make_rw_test<double>("EZminc_double_3_short.mnc",3,NC_SHORT,true,0.1);
    
    make_rw_test<float>("EZminc_float_2_byte.mnc",2,NC_BYTE,false,0.5);
    make_rw_test<float>("EZminc_float_3_byte.mnc",3,NC_BYTE,false,0.5);
    
    make_rw_test<double>("EZminc_double_2_byte.mnc",2,NC_BYTE,false,0.5);
    make_rw_test<double>("EZminc_double_3_byte.mnc",3,NC_BYTE,false,0.5);
    
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

