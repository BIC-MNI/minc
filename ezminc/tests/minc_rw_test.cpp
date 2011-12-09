#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"

using namespace minc;

template<class T>class volume_3d
{
  protected:
    T* _data;
    int _x,_y,_z;
  public:
    
    volume_3d(T* data,int x,int y,int z):_data(data),_x(x),_y(y),_z(z)
    {
    }
    
    volume_3d(std::vector<T> data,int x,int y,int z):_data(&data[0]),_x(x),_y(y),_z(z)
    {
    }
    
    T& operator()(int x,int y,int z)
    {
      return _data[x+y*_x+z*_x*_y];
    }
  
};

int main(int argc,char **argv)
{
  try
  {
    if(argc<4) {
      std::cerr<<"Usage: "<<argv[0]<<" <input.mnc> <output.mnc> <output2.mnc>"<<std::endl;
      return 1;
    }
    minc_1_reader rdr;
    rdr.open(argv[1]);
    int i;
    for(i=0;i<rdr.dim_no();i++)
      std::cout<<rdr.dim(i).name<<" "<<rdr.dim(i).length<<std::endl;
    std::cout<<"Slice len="<<rdr.slice_len()<<std::endl;
    std::cout<<"History:"<<rdr.history().c_str()<<std::endl;
    for(int v=0;v<rdr.var_number();v++)
    {
      std::string var=rdr.var_name(v);
      for(int a=0;a<rdr.att_number(v);a++)
      {
        std::string aname=rdr.att_name(v,a);
        nc_type dt=rdr.att_type(v,aname.c_str());
        
        std::cout<<var.c_str()<<":"<<aname.c_str()<<" ";
        if(dt==NC_CHAR) 
          std::cout<<rdr.att_value_string(v,aname.c_str());
        else if(dt=NC_DOUBLE) 
        {
          std::vector<double> val=rdr.att_value_double(v,aname.c_str());
          for(int d=0;d<val.size();d++)
            std::cout<<val[d]<<"\t";
        } else {
          std::cout<<"???";
        }
        std::cout<<std::endl;
      }
    }
    
    rdr.setup_read_float();
    
    minc_1_writer wrt;
    wrt.open(argv[2],rdr.info(),2,NC_FLOAT,true);
    wrt.setup_write_float();
    std::vector<float> v(rdr.slice_len());

    int c=0;
    double s=0;
    for(rdr.begin(),wrt.begin();!rdr.last();rdr.next_slice(),wrt.next_slice())
    {
      //std::cout<<wrt.current_slice()[0]<<std::endl;
      rdr.read(&v[0]);
      for(int i=0;i<rdr.slice_len();i++)
      {
        s+=v[i];
        c++;
      }
      wrt.write(&v[0]);
    }
    s/=c;
    //std::cout<<wrt._image_range[0]<<" "<<wrt._image_range[1]<<std::endl;
    std::cout<<s<<std::endl;
    wrt.copy_headers(rdr);
    wrt.append_history("minc_rw_test test1\n");
    
    //second test
    unsigned long size=1;
    for(i=0;i<rdr.dim_no();i++)
      size*=rdr.dim(i).length;
    
    minc_1_writer wrt2;
    wrt2.open(argv[3],rdr.info(),3,NC_SHORT,false);
    wrt2.setup_write_float();
    
    std::vector<float> buffer(size);
    load_standard_volume<float>(rdr,&buffer[0]);
    double avg2=0;
    //for(int i=0;i<size;i++)
    //  avg2+=buffer[i];
    volume_3d<float> volume(buffer,rdr.ndim(dim_info::DIM_X),rdr.ndim(dim_info::DIM_Y),rdr.ndim(dim_info::DIM_Z));
    for(int z=0;z<rdr.ndim(dim_info::DIM_Z);z++)
      for(int y=0;y<rdr.ndim(dim_info::DIM_Y);y++)
        for(int x=0;x<rdr.ndim(dim_info::DIM_X);x++)
        {
          avg2+=volume(x,y,z);
        }
    
    avg2/=size;
    std::cout<<"avg2 "<<avg2<<std::endl;
    
    save_standard_volume<float>(wrt2,&buffer[0]);
    wrt2.copy_headers(rdr);
    wrt2.append_history("minc_rw_test test2\n");
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

