#ifndef __MINC_1_SIMPLE_H__
#define __MINC_1_SIMPLE_H__

#include "minc_1_rw.h"

namespace minc
{
  
  template <class T> class minc_input_iterator
  {
    protected:
      mutable minc_1_reader* _rw;
      std::vector<T> _buf;
      std::vector<long> _cur;
      bool _last;
      size_t _count;
    public:
      
    const std::vector<long>& cur(void) const
    {
      return _cur;
    }
    
    
    minc_input_iterator(const minc_input_iterator<T>& a):_rw(a._rw),_cur(a._cur),_last(a._last),_count(a._count)
    {
    }
    
    minc_input_iterator(minc_1_reader& rw):_rw(&rw),_last(false),_count(0)
    {
    }
    
    minc_input_iterator():_rw(NULL),_last(false),_count(0)
    {
    }
    
    void attach(minc_1_reader& rw)
    {
      _rw=&rw;
      _last=false;
      _count=0;
    }
    
    bool next(void)
    {
      if(_last) return false;
      _count++;
      for(size_t i=static_cast<size_t>(_rw->dim_no()-1);
	  i>static_cast<size_t>(_rw->dim_no()-_rw->slice_dimensions()-1);i--)
      {
        _cur[i]++;
        if(_cur[i]<static_cast<long>(_rw->dim(i).length))
          break;
        if(i>static_cast<size_t>(_rw->dim_no()-_rw->slice_dimensions())) 
          _cur[i]=0;
        else
        {
          //move to next slice 
          if(i==0) // the case when slice_dimensions==dim_no
          {
            _last=true;
            _count=0;
            break;
          }
          if(!_rw->next_slice())
          {
            _last=true;
            break;
          }
          _rw->read(&_buf[0]);
          _cur=_rw->current_slice();
          _count=0;
          break;
        }
      }
      return !_last;
    }
    
    bool last(void)
    {
      return _last;
    }
    
    void begin(void)
    {
      _cur.resize(MAX_VAR_DIMS,0);
      _buf.resize(_rw->slice_len());
      _count=0;
      _rw->begin();
      _rw->read(&_buf[0]);
      _cur=_rw->current_slice();
    }
    
    const T& value(void) const
    {
      return _buf[_count];
    }
  };
  
  template <class T> class minc_output_iterator
  {
    protected:
      mutable minc_1_writer* _rw;
      std::vector<T> _buf;
      std::vector<long> _cur;
      bool _last;
      size_t _count;
    public:
    const std::vector<long>& cur(void) const
    {
      return _cur;
    }
    
    minc_output_iterator(const minc_output_iterator<T>& a):_rw(a._rw),_cur(a._cur),_last(a._last),_count(a._count)
    {
    }
    
    minc_output_iterator(minc_1_writer& rw):_rw(&rw),_last(false),_count(0)
    {
      _buf.resize(rw.slice_len()); 
    }
    
    minc_output_iterator():_rw(NULL),_last(false),_count(0)
    {
    }
    
    void attach(minc_1_writer& rw)
    {
      _rw=&rw;
      _last=false;
      _count=0;
    }
    
    ~minc_output_iterator()
    {
      if(_count && !_last)
        _rw->write(&_buf[0]);
    }
    
    bool next(void)
    {
      if(_last) return false;
      _count++;
      for(int i=_rw->dim_no()-1;i>(_rw->dim_no()-_rw->slice_dimensions()-1);i--)
      {
        _cur[i]++;
        if(_cur[i]<static_cast<long>(_rw->dim(i).length))
          break;
        if(i>(_rw->dim_no()-_rw->slice_dimensions())) 
          _cur[i]=0;
        else
        {
          //write slice into minc file
          _rw->write(&_buf[0]);
          _count=0;
          //move to next slice 
          if(i==0) // the case when slice_dimensions==dim_no
          {
            _last=true;
            return false;
          }
          if(!_rw->next_slice())
          {
            _last=true;
            break;
          }
          _cur=_rw->current_slice();
          break;
        }
      }
      return !_last;
    }
    
    bool last(void)
    {
      return _last;
    }
    
    void begin(void)
    {
      _buf.resize(_rw->slice_len());
      _cur.resize(MAX_VAR_DIMS,0);
      _count=0;
      _rw->begin();
      _cur=_rw->current_slice();
    }
    
    void value(const T& v) 
    {
      _buf[_count]=v;
    }
  };
  
  //! will attempt to laod the whole volume in T Z Y X V order into buffer, file should be prepared (setup_read_XXXX)
  template<class T> void load_standard_volume(minc_1_reader& rw, T* volume)
  {
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    for(size_t i=0;i<5;i++)
    {      
      if(rw.map_space(i)<0) continue;
      strides[rw.map_space(i)]=str;
      str*=rw.ndim(i);
    }

    minc_input_iterator<T> in(rw);
    for(in.begin();!in.last();in.next())
    {
      size_t address=0;
      for(size_t i=0;i<static_cast<size_t>(rw.dim_no());i++)
        address+=in.cur()[i]*strides[i];
        
      volume[address]=in.value();
    }
  }
  
  //! will attempt to save the whole volume in T Z Y X V order from buffer, file should be prepared (setup_read_XXXX)
  template<class T> void save_standard_volume(minc_1_writer& rw, const T* volume)
  {
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    for(size_t i=0;i<5;i++)
    {      
      if(rw.map_space(i)<0) continue;
      strides[rw.map_space(i)]=str;
      str*=rw.ndim(i);
    }
    
    minc_output_iterator<T> out(rw);
    for(out.begin();!out.last();out.next())
    {
      size_t address=0;
      for(size_t i=0;i<static_cast<size_t>(rw.dim_no());i++)
        address+=out.cur()[i]*strides[i];
        
      out.value(volume[address]);
    }
  }

  //! will attempt to load the whole volume in Z Y X T V  order into buffer, file should be prepared (setup_read_XXXX)
  template<class T> void load_non_standard_volume(minc_1_reader& rw, T* volume)
  {
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    const size_t dimorder[]={0,4,1,2,3};
    for(size_t i=0;i<5;i++)
    {
      if(rw.map_space(dimorder[i])<0|| !rw.ndim(dimorder[i]) ) continue;
      strides[rw.map_space(dimorder[i])]=str;
      str*=rw.ndim(dimorder[i]);
    }
    
    minc_input_iterator<T> in(rw);
    for(in.begin();!in.last();in.next())
    {
      size_t address=0;
      for(size_t i=0;i<rw.dim_no();i++)
        address+=in.cur()[i]*strides[i];
      
      volume[address]=in.value();
    }
  }
  
  //! will attempt to save the whole volume in V T Z Y X order from buffer, file should be prepared (setup_read_XXXX)
  template<class T> void save_non_standard_volume(minc_1_writer& rw, const T* volume)
  {
    std::vector<size_t> strides(MAX_VAR_DIMS,0);
    size_t str=1;
    const size_t dimorder[]={0,4,1,2,3};
    for(size_t i=0;i<5;i++)
    {
      if(rw.map_space(dimorder[i])<0 || !rw.ndim(dimorder[i]) ) continue;
      strides[rw.map_space(dimorder[i])]=str;
      str*=rw.ndim(dimorder[i]);
    }
    minc_output_iterator<T> out(rw);
    for(out.begin();!out.last();out.next())
    {
      size_t address=0;
      for(size_t i=0;i<rw.dim_no();i++)
        address+=out.cur()[i]*strides[i];

      out.value(volume[address]);
    }
  }
  
  
};//minc

#endif //__MINC_1_SIMPLE_H__
