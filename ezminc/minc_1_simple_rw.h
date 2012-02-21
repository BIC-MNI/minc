#ifndef __MINC_1_SIMPLE_RW_H__
#define __MINC_1_SIMPLE_RW_H__

#include "minc_1_simple.h"
#include "minc_io_simple_volume.h"
#include "minc_io_fixed_vector.h"
#include "minc_io_4d_volume.h"

namespace minc
{
  
  template<class T> void load_simple_volume(minc_1_reader& rw,simple_volume<T>& vol)
  {
    if(rw.ndim(1)<=0||rw.ndim(2)<=0||rw.ndim(3)<=0||rw.ndim(4)>0) 
      REPORT_ERROR("Need 3D minc file");
    
    vol.resize(rw.ndim(1),rw.ndim(2),rw.ndim(3));
     
    if(typeid(T)==typeid(unsigned char))
    {
      rw.setup_read_byte();
      load_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(int))
    {
      rw.setup_read_int();
      load_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(fixed_vec<3,float>))
    {
      rw.setup_read_float(); 
      load_standard_volume<float>(rw,(float*)vol.c_buf());
    }
    else if(typeid(T)==typeid(float))
    {
      rw.setup_read_float(); 
      load_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(fixed_vec<3,double>))
    {
      rw.setup_read_double(); 
      load_standard_volume<double>(rw,(double*)vol.c_buf());
    }
    else if(typeid(T)==typeid(double))
    {
      rw.setup_read_double(); 
      load_standard_volume(rw,vol.c_buf());
    } else 
			REPORT_ERROR("Data type not supported for minc io");
    
    //set coordinate transfer parameters
    for(int i=0;i<3;i++)
    {
      vol.step()[i]=rw.nspacing(i+1);
      vol.start()[i]=rw.nstart(i+1);
      
      if(rw.have_dir_cos(i+1))
      {
        for(int j=0;j<3;j++)
          vol.direction_cosines(i)[j]=rw.ndir_cos(i+1,j);
      } else {
        for(int j=0;j<3;j++)
          vol.direction_cosines(i)[j]=(i==j?1.0:0.0); //identity
      }
    }
  }
  
  template<class T> void save_simple_volume(minc_1_writer& rw,const simple_volume<T>& vol)
  {
    if(typeid(T)==typeid(unsigned char))
    {
      rw.setup_write_byte();
      save_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(int))
    {
      rw.setup_write_int();
      save_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(fixed_vec<3,float>))
    {
      rw.setup_write_float(); 
      save_standard_volume<float>(rw,(float*)vol.c_buf());
    }
    else if(typeid(T)==typeid(float))
    {
      rw.setup_write_float(); 
      save_standard_volume(rw,vol.c_buf());
    }
    else if(typeid(T)==typeid(fixed_vec<3,double>))
    {
      rw.setup_write_double(); 
      save_standard_volume<double>(rw,(double*)vol.c_buf());
    }
    else if(typeid(T)==typeid(double))
    {
      rw.setup_write_double(); 
      save_standard_volume(rw,vol.c_buf());
    }
    else 
			REPORT_ERROR("Data type not supported for minc io");
  }
  
  
  template<class T> void load_4d_volume(minc_1_reader& rw,simple_4d_volume<T>& vol)
  {
    //if(rw.ndim(1)<=0||rw.ndim(2)<=0||rw.ndim(3)<=0||rw.ndim(4)<=0) 
    //  REPORT_ERROR("Need 4D minc file");
    
    vol.resize(rw.ndim(1),rw.ndim(2),rw.ndim(3),rw.ndim(4)>0?rw.ndim(4):1); //always assume 4 dimensions
     
    if(typeid(T)==typeid(unsigned char))
      rw.setup_read_byte();
    else if(typeid(T)==typeid(int))
      rw.setup_read_int();
    else if(typeid(T)==typeid(fixed_vec<3,float>))
      rw.setup_read_float(); 
    else if(typeid(T)==typeid(float))
      rw.setup_read_float(); 
    else if(typeid(T)==typeid(fixed_vec<3,double>))
      rw.setup_read_double(); 
    else if(typeid(T)==typeid(double))
      rw.setup_read_double();
		else 
			REPORT_ERROR("Data type not supported for minc io");
    
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    
    for(size_t i=0;i<5;i++) //T is a special case
    {      
      if(rw.map_space(i)<0) continue;
      strides[rw.map_space(i)]=str;
      str*=rw.ndim(i);
    }
    
    if(rw.map_space(4)>=0)
      strides[rw.map_space(4)]=0; //t dimension

    minc_input_iterator<T> in(rw);
    for(in.begin();!in.last();in.next())
    {
      size_t address=0;
      size_t slice=0;
      for(size_t i=0;i<rw.dim_no();i++)
      {
        if(strides[i]>0)
          address+=in.cur()[i]*strides[i];
        else //
          slice=in.cur()[i];
      }
      vol.frame(slice).c_buf()[address]=in.value();
    }
    
    //set coordinate transfer parameters
    for(int i=0;i<3;i++)
    {
      vol.step()[i]=rw.nspacing(i+1);
      vol.start()[i]=rw.nstart(i+1);
      
      if(rw.have_dir_cos(i+1))
      {
        for(int j=0;j<3;j++)
          vol.direction_cosines(i)[j]=rw.ndir_cos(i+1,j);
      } else {
        for(int j=0;j<3;j++)
          vol.direction_cosines(i)[j]=(i==j?1.0:0.0); //identity
      }
    }
    if(rw.ndim(4)>0)
    {
      vol.t_start()=rw.nstart(4);//T
      vol.t_step()=rw.nspacing(4);//T
    } else {
      vol.t_start()=0;//T
      vol.t_step()=0;//T
    }
  }
  
  template<class T> void save_4d_volume(minc_1_writer& rw,const simple_4d_volume<T>& vol)
  {
    if(typeid(T)==typeid(unsigned char))
      rw.setup_write_byte();
    else if(typeid(T)==typeid(int))
      rw.setup_write_int();
    else if(typeid(T)==typeid(fixed_vec<3,float>))
      rw.setup_write_float(); 
    else if(typeid(T)==typeid(float))
      rw.setup_write_float(); 
    else if(typeid(T)==typeid(fixed_vec<3,double>))
      rw.setup_write_double(); 
    else if(typeid(T)==typeid(double))
      rw.setup_write_double(); 
    else 
			REPORT_ERROR("Data type not supported for minc io"); 
		
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    for(size_t i=0;i<4;i++)//T is a special
    {      
      if(rw.map_space(i)<0) continue;
      strides[rw.map_space(i)]=str;
      str*=rw.ndim(i);
    }
    
    if(rw.map_space(4)>=0)
      strides[rw.map_space(4)]=0; //t dimension
    
    minc_output_iterator<T> out(rw);
    for(out.begin();!out.last();out.next())
    {
      size_t address=0;
      size_t slice=0;
      for(size_t i=0;i<rw.dim_no();i++)
      {
        if(strides[i]>0)
          address+=out.cur()[i]*strides[i];
        else //
          slice=out.cur()[i];
      }
      out.value(vol.frame(slice).c_buf()[address]);
    }
  }
  
  bool is_same(minc_1_reader& one,minc_1_reader& two,bool verbose=true);
  
  template<class T> void load_minc_file(const char *file,simple_4d_volume<T>& vol)
  {
      minc_1_reader rdr;
      rdr.open(file);
      load_4d_volume(rdr,vol);  
  }
  
  template<class T> void generate_info(const simple_4d_volume<T>& vol,minc_info& info)
  {
     bool have_time=vol.frames()>1||vol.t_step()!=0.0; //assume that it is 3D file otherwise
     
     bool is_vector=false;
      
      if(typeid(T)==typeid(fixed_vec<3,float>)) {
        is_vector=true;
      } 
      
      info.resize(3+(is_vector?1:0)+(have_time?1:0));
      
      if(is_vector)
      {
        info[0].dim=dim_info::DIM_VEC;
        info[0].length=3;
        info[0].step=1;
        
      }
      
      for(int i=0;i<3;i++)
      {
        int ii=i+(is_vector?1:0);
        info[ii].dim=dim_info::dimensions( dim_info::DIM_X+i);
        
        info[ii].length=vol.dim(i);
        info[ii].step  =vol.step()[i];
        info[ii].start =vol.start()[i];
        info[ii].have_dir_cos=true;
        
        for(int j=0;j<3;j++)
          info[ii].dir_cos[j]=vol.direction_cosines(i)[j];
      }
      
      if(have_time) 
      {
        info[3+(is_vector?1:0)].dim=dim_info::DIM_TIME;
        info[3+(is_vector?1:0)].step=vol.t_step();
        info[3+(is_vector?1:0)].start=vol.t_start();
        info[3+(is_vector?1:0)].length=vol.frames();
      }
          
  }
  
  template<class T> void save_minc_file(const char *file,const simple_4d_volume<T>& vol,
                                        const char* history=NULL,const minc_1_reader* original=NULL,
                                        nc_type datatype=NC_NAT,bool is_signed=false)
  {
      minc_1_writer wrt;
      //convert parameters to info
      
      if(typeid(T)==typeid(unsigned char))
      {
        if(datatype==NC_NAT) datatype=NC_BYTE;
        
      } else if(typeid(T)==typeid(int)) {
        if(datatype==NC_NAT) datatype=NC_INT;
        
        is_signed=true;
      } else if(typeid(T)==typeid(unsigned int))  {
        if(datatype==NC_NAT) datatype=NC_INT;
        
        is_signed=false;
      } else if(typeid(T)==typeid(float))  {
        if(datatype==NC_NAT) datatype=NC_FLOAT;
        
        is_signed=true;
      } else if(typeid(T)==typeid(fixed_vec<3,float>)) {
        if(datatype==NC_NAT) datatype=NC_FLOAT;
        
        is_signed=true;
      } else if(typeid(T)==typeid(double))  {
        if(datatype==NC_NAT) datatype=NC_DOUBLE;
        
        is_signed=true;
      } else if(typeid(T)==typeid(fixed_vec<3,double>)) {
        if(datatype==NC_NAT) datatype=NC_DOUBLE;
        
        is_signed=true;
      } else
        REPORT_ERROR("Unsupported data type!");
      
      minc_info info;
      generate_info<T>(vol,info);
      
      wrt.open(file,info,2,datatype,is_signed);
      
      if(original)
      {
        wrt.copy_headers(*original);
      }
      
      if(history)
        wrt.append_history(history);
      
      save_4d_volume(wrt,vol);
  }
  
};

#endif //__MINC_1_SIMPLE_RW_H__
