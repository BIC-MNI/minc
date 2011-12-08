#ifndef __SIMPLE_VOLUME_H__
#define __SIMPLE_VOLUME_H__

#include "minc_io_exceptions.h"
#include "minc_io_fixed_vector.h"
#include <string.h>
#include <math.h>

namespace minc 
{
  //! very simple 3D volume, initially created as an example but became usable
    template<class T> class simple_volume
    {
    public:
    
      enum    {ndims=3};
      typedef fixed_vec<ndims,int> idx;
      typedef fixed_vec<ndims,double> vect;
      
    protected:
    
      T * _vol;    //! the volume itself
      idx _size;   //! dimension sizes
      idx _stride; //! used internally 
      int _count;  //! total number of voxels
      bool _free_memory; //! should the array be freed
      
      vect _step,_start;    //! conversion to wold coordinates
      vect _direction_cosines[3];
      
      
      void _allocate(T* data=NULL)
      {
        _stride[0]=1;
        int total=_size[0];
        for(int i=1;i<ndims;i++)
        {
          _stride[i]=_size[i-1]*_stride[i-1];
          total*=_size[i];
        }
        _count=total;
        if(data)
        {
          _vol=data;
          _free_memory=false;
        } else {
          _vol=new T[total];
          _free_memory=true;
        }
        
        _step=IDX<double>(1.0,1.0,1.0);
        _start=IDX<double>(0.0,0.0,0.0);
        
        _direction_cosines[0]=IDX<double>(1.0,0.0,0.0);
        _direction_cosines[1]=IDX<double>(0.0,1.0,0.0);
        _direction_cosines[2]=IDX<double>(0.0,0.0,1.0);
        
      }

    public:
      vect& start(void)
      {
        return _start;
      }
      
      const vect& start(void) const
      {
        return _start;
      }

      vect& step(void)
      {
        return _step;
      }
      
      
      const vect& step(void) const
      {
        return _step;
      }
      
      vect& direction_cosines(int i)
      {
        return _direction_cosines[i];
      }
      
      const vect& direction_cosines(int i) const
      {
        return _direction_cosines[i];
      }

      operator T*() 
      {
        return _vol;
      }
      
      T* c_buf() 
      {
        return _vol;
      }
      
      const T* c_buf() const
      {
        return _vol;
      }
      
      int c_buf_size() const
      {
        return _count;
      }

      explicit simple_volume(const int* dims):_vol(0),_size(dims)
      {
        _allocate();
      }
      
      simple_volume(const simple_volume<T>& a,bool copy_data=true):_vol(0)
      {
        for(int i=0;i<ndims;i++) 
          _size[i]=a._size[i];
        _allocate();
        
        if(copy_data)
        {
          memmove(_vol,a._vol,_size[0]*_size[1]*_size[2]*sizeof(T));
        }
        
        _step=a._step;
        _start=a._start;
        for(int i=0;i<ndims;i++)
          _direction_cosines[i]=a._direction_cosines[i];
      }
      
      simple_volume(int sx,int sy,int sz):_vol(0)
      {
        _size=IDX(sx,sy,sz);
        _allocate();
      }
      
      explicit simple_volume(const idx& s):_vol(0)
      {
        _size=s;
        _allocate();
      }

      simple_volume():_vol(0)
      {
        for(int i=0;i<ndims;i++)
        {
          _size[i]=0;
          _step[i]=0.0;
          _start[i]=0.0;
          
          _direction_cosines[i]=IDX<double>(0.0,0.0,0.0);
          _direction_cosines[i][i]=1.0;
        }
      }
      
      bool empty(void) const
      {
        return !_size[0]||!_vol;
      }
      
      void resize(int sx,int sy,int sz)
      {
        if( _size[0]==sx && _size[1]==sy && _size[2]==sz )
          return;
        
        if(_vol && _free_memory)
          delete [] _vol;
        _size=IDX(sx,sy,sz);
        _allocate();
      }
      
      void resize(const idx& s)
      {
        if(_size==s) return;
        
        if(_vol&&_free_memory)
          delete [] _vol;
        _size=s;
        _allocate();
      }
      
      virtual ~simple_volume()
      {
        if(_vol && _free_memory)
          delete [] _vol;
      }
      
      T& operator()(int x,int y,int z)
      {
        return _vol[x+y*_stride[1]+z*_stride[2]];
      }
      
      T& operator()(const idx& i)
      {
        return _vol[dot(i,_stride)];
      }
      
      const T& operator()(int x,int y,int z) const
      {
        return get(x,y,z);
      }
      
      const T& operator()(const idx& i) const
      {
        return get(i);
      }
      
      const T& get(int x,int y,int z) const
      {
        return _vol[x+y*_stride[1]+z*_stride[2]];
      }
      
      const T& get(const idx& i) const
      {
        return _vol[dot(i,_stride)];
      }
      
      const T& safe_get(int x,int y,int z) const
      {
        check_index(x,y,z);
        return _vol[x+y*_stride[1]+z*_stride[2]];
      }
     
      const T& safe_get(idx i) const
      {
        check_index(i);
        return get(i);
      }

      //trilinear intrpolation
      double interpolate(float _x,float _y,float _z) const
      {
        int x=floor(_x);
        int y=floor(_y);
        int z=floor(_z);
        
        float dx=_x-x;
        float dy=_y-y;
        float dz=_z-z;
        
        if(x<0) x=-x;
        if(y<0) y=-y;
        if(z<0) z=-z;
        
        if(x>=(_size[0]-1)) x=_size[0]*2-3-x;
        if(y>=(_size[1]-1)) y=_size[1]*2-3-y;
        if(z>=(_size[2]-1)) z=_size[2]*2-3-z;
        
        //trilinear intrpolation
        return     (1.0-dx)*(1.0-dy)*(1.0-dz)*get(x,y,z)+
        
                   dx*(1.0-dy)*(1.0-dz)*get(x+1,y,z)+
                   (1.0-dx)*dy*(1.0-dz)*get(x,y+1,z)+
                   (1.0-dx)*(1.0-dy)*dz*get(x,y,z+1)+
                   
                   dx*(1.0-dy)*dz*get(x+1,y,z+1)+
                   (1.0-dx)*dy*dz*get(x,y+1,z+1)+
                   dx*dy*(1.0-dz)*get(x+1,y+1,z)+
                   
                   dx*dy*dz*get(x+1,y+1,z+1);
      }
      
      T set(int x,int y,int z, const T&v)
      {
        return _vol[x+y*_stride[1]+z*_stride[2]]=v;
      }
      
      T set(const idx& i, const T&v)
      {
        return _vol[dot(i,_stride)]=v;
      }
      
      int dim(int i) const
      {
        return _size[i];
      }
      
      const int* dims() const
      {
        return _size.c_buf();
      }
      
      const idx& size() const
      {
        return _size;
      }
      
      void extract_subvolume(simple_volume<T>& dst,const idx& s, const idx& f) const
      {
        for(int k=s[2];k<f[2];k++)
         for(int j=s[1];j<f[1];j++)
          for(int i=s[0];i<f[0];i++)
            dst(i,j,k)=get(i,j,k);
      }
      
      void check_index(int &ii,int &jj,int &kk) const
      {
        if(ii<0) ii=-ii;
        if(jj<0) jj=-jj;
        if(kk<0) kk=-kk;

        if(ii>=dim(0)) ii=2*dim(0)-ii-1;
        if(jj>=dim(1)) jj=2*dim(1)-jj-1;
        if(kk>=dim(2)) kk=2*dim(2)-kk-1;
      }
      
      void check_index(idx& iii)
      {
        for(int i=0;i<3;i++)
        {
          if(iii[i]<0) 
            iii[i]=-iii[i];

          if(iii[i]>=dim(i)) 
            iii[i]=2*dim(i)-iii[i]-1;

        }
      }

      bool hit(int ii,int jj,int kk) const
      {
        if(ii<0) return false;
        if(jj<0) return false;
        if(kk<0) return false;

        if(ii>=dim(0)) return false;
        if(jj>=dim(1)) return false;
        if(kk>=dim(2)) return false;
        return true;
      }
      
      bool hit(const idx iii) const
      {
        for(int i=0;i<3;i++)
        {
          if(iii[i]<0) return false;
          if(iii[i]>=dim(i)) return false;
        }
        return true;
      }
          
      simple_volume<T>& operator+=(const simple_volume<T>& a)
      {
        for(int i=0;i<ndims;i++) 
          if(_size[i]!=a._size[i])
            REPORT_ERROR("Dimensions are different");
        
        for(int i=0;i<_count;i++)
          _vol[i]+=a._vol[i];
        return *this;
      }
      
      simple_volume<T>& operator+=(const T& a)
      {
        for(int i=0;i<_count;i++)
          _vol[i]+=a;
        return *this;
      }
      
      simple_volume<T>& operator-=(const simple_volume<T>& a)
      {
         if(_size!=a._size)
            REPORT_ERROR("Dimensions are different");
        
        for(int i=0;i<_count;i++)
          _vol[i]-=a._vol[i];
        return *this;
      }
      
      simple_volume<T>& operator-=(const T& a)
      {
        for(int i=0;i<_count;i++)
          _vol[i]-=a;
        return *this;
      }
      
      simple_volume<T>& operator*=(const simple_volume<T>& a)
      {
        for(int i=0;i<ndims;i++) 
          if(_size[i]!=a._size[i])
            REPORT_ERROR("Dimensions are different");
        
        for(int i=0;i<_count;i++)
          _vol[i]*=a._vol[i];
        return *this;
      }
      
      simple_volume<T>& operator*=(const T& a)
      {
        for(int i=0;i<_count;i++)
          _vol[i]*=a;
        return *this;
      }
      
      simple_volume<T>& operator/=(const simple_volume<T>& a)
      {
        if(_size!=a._size())
          REPORT_ERROR("Dimensions are different");
        
        for(int i=0;i<_count;i++)
          _vol[i]/=a._vol[i];
        return *this;
      }
      
      simple_volume<T>& operator/=(const T& a)
      {
        for(int i=0;i<_count;i++)
          _vol[i]/=a;
        return *this;
      }
      
      simple_volume& operator=(const simple_volume<T>&a)
      {
        resize(a.dim(0),a.dim(1),a.dim(2));
        
        memmove(_vol, a._vol, _count*sizeof(T));
        
        _step=a._step;
        _start=a._start;
        
        for(int i=0;i<ndims;i++)
          _direction_cosines[i]=a._direction_cosines[i];
        
        return *this;
      }
      
      simple_volume& operator=(const T&a)
      {
        for(int i=0;i<_count;i++)
          _vol[i]=a;
        return *this;
      }
      
      void weighted_add(const simple_volume<T>&a, double w)
      {
        if(_size!=a._size)
          REPORT_ERROR("Dimensions are different");
        
        for(int i=0;i<_count;i++)
          _vol[i]+=a._vol[i]*w;
      }
      
      vect voxel_to_world(const idx& iii) const
      {
        vect ret=IDX<double>(0.0,0.0,0.0);
        for(int i=0;i<ndims;i++)
        {
          for(int j=0;j<ndims;j++)
            ret[i]+=(_step[j]*iii[j]+_start[j])*_direction_cosines[j][i];
        }
        return ret;
      }

      vect world_to_voxel_c(const vect& iii) const
      {
        vect ret=IDX<double>(0.0,0.0,0.0);
        for(int i=0;i<ndims;i++)
        {
          for(int j=0;j<ndims;j++)
            ret[i]+=((iii[j]-_start[j])/_step[j])*_direction_cosines[i][j]; //transpose!
        }
        return ret;
      }

      idx world_to_voxel(const vect& iii) const
      {
        vect ret=world_to_voxel_c(iii);
        
        idx r;
        
        for(int i=0;i<ndims;i++)
          r[i]=floor(ret[i]+0.5);
        
        return r;
      }
      
      //!use provided buffer for storage
      void assign(const idx& s,T* array) 
      {
        _size=s;
        allocate(array);
      }
      
  };
  
  //! remove (unpad) or add padding as needed, volume will be centered
  template<class T>void pad_volume(const simple_volume<T> &src,simple_volume<T> &dst, const T& fill)
  {
    fixed_vec<3,int> sz1=src.size();
    fixed_vec<3,int> sz2=dst.size();
    fixed_vec<3,int> d=sz2-sz1;
    fixed_vec<3,int> i;
    
    d/=2;//offset
  
    for( i[2]=0;i[2]<sz2[2];i[2]++)
      for( i[1]=0;i[1]<sz2[1];i[1]++)
        for( i[0]=0;i[0]<sz2[0];i[0]++)
    {
      fixed_vec<3,int> j= i-d;
      
      if( src.hit(j)) 
        dst.set(i,src.get(j));
      else
        dst.set(i,fill);
    }
  }
  
  template<class T>void volume_min_max(const simple_volume<T>& v,T &_min,T &_max)
  {
    _min=_max=v.c_buf()[0];
    
    for(int i=0;i<v.c_buf_size();i++)
    {
			if(isnan(v.c_buf()[i]) || isinf(v.c_buf()[i])) 
				continue;
			
      if(v.c_buf()[i]>_max) _max=v.c_buf()[i];
      else if(v.c_buf()[i]<_min) _min=v.c_buf()[i];
    }
  }
  
  template<class T> void  volume_min_max(const simple_volume<T>& v,const simple_volume<unsigned char>& mask,T &_min,T &_max)
  {
    if(v.size()!=mask.size())
      REPORT_ERROR("Volume size mismatch");
    
    _min=1e10;
    _max=-1e10;
    
    for(int i=0;i<v.c_buf_size();i++)
    {
      if(mask.c_buf()[i])
      {
				if(isnan(v.c_buf()[i]) || isinf(v.c_buf()[i])) 
					continue;
        if(v.c_buf()[i]>_max) _max=v.c_buf()[i];
        else if(v.c_buf()[i]<_min) _min=v.c_buf()[i];
      }
    }
  }
  
  
  typedef simple_volume<float>               minc_float_volume;
  typedef simple_volume<fixed_vec<3,float> > minc_grid_volume;
  typedef simple_volume<unsigned char>       minc_byte_volume;

}; //minc


#endif // __SIMPLE_VOLUME_H__
