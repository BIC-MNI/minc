#ifndef __MINC_IO_4D_VOLUME_H_

#include "minc_io_simple_volume.h"
#include <vector>
#include <cstring>

namespace minc
{
  
  //! simple 4D volume - collection of 3D volumes
  template<class T> class simple_4d_volume
  {
    protected:
      enum    {ndims=3};
      typedef fixed_vec<ndims,int> idx;
      typedef fixed_vec<ndims,double> vect;
      
      double _start_t;
      double _step_t;
      
      void allocate(int n,const idx& sz)
      {
        _volumes.resize(n);
        for(int i=0;i<n;i++)
        {
          _volumes[i].resize(sz);
        }
      }
      
    public:
      typedef simple_volume<T> volume;
      typedef std::vector<volume> volume_list;
      
      int dim(int i) const
      {
        return _volumes[0].dim(i);
      }
      
      vect voxel_to_world(const idx& iii) const
      {
        return _volumes[0].voxel_to_world(iii);
      }
      
      idx world_to_voxel(const vect& iii) const
      {
        return _volumes[0].world_to_voxel(iii);
      }
      
      void resize(int x,int y,int z,int t)
      {
        allocate(t,IDX<int>(x,y,z));
      }
      
      //! number of temporal frames
      size_t  frames(void) const
      {
        return _volumes.size();
      }
      
      T& at(int x,int y,int z,int t)
      {
        return _volumes[t].at(x,y,z);
      }
      
      const T& get(int x,int y,int z,int t) const
      {
        return _volumes[t].get(x,y,z);
      }
      
      void set(int x,int y,int z,int t,const T& v)
      {
        _volumes[t].set(x,y,z,v);
      }
      
      T& at(const idx& i,int t)
      {
        return _volumes[t].at(i);
      }

      const T& get(const idx& i,int t) const
      {
        return _volumes[t].get(i);
      }
      
      void set(const idx& i,int t,const T& v)
      {
        _volumes[t].set(i,v);
      }
      
      volume& frame(int t) 
      {
        return _volumes[t];
      }

      const volume& frame(int t) const
      {
        return _volumes[t];
      }
      
      vect& start(void)
      {
        return _volumes[0].start();
      }
      
      const vect& start(void) const
      {
        return _volumes[0].start();
      }
      
      vect& step(void)
      {
        return _volumes[0].step();
      }
      
      const vect& step(void) const
      {
        return _volumes[0].step();
      }
      
      vect& direction_cosines(int i)
      {
        return _volumes[0].direction_cosines(i);
      }
      
      const vect& direction_cosines(int i) const
      {
        return _volumes[0].direction_cosines(i);
      }
      
      double & t_step(void)
      {
        return _step_t;
      }
      
      double t_step(void) const
      {
        return _step_t;
      }
      
      double & t_start(void)
      {
        return _start_t;
      }
      
      double t_start(void) const
      {
        return _start_t;
      }
      
    protected:

      volume_list _volumes;
  }; 

};

#endif //__MINC_IO_4D_VOLUME_H_
