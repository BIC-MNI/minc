#ifndef __MINC_1_RW__
#define __MINC_1_RW__

#include <vector>
#include <string>

#include "minc_io_exceptions.h"
//#include "minc_io_fixed_vector.h"

#ifdef USE_MINC2
#define MINC2 1
#endif 

extern "C" {
#include <minc.h> 
}

#include <typeinfo>
#include <float.h>
#include <iostream>

namespace minc
{
  
  //! class for storing dimension information 
  struct dim_info
  {
    enum dimensions {DIM_UNKNOWN=0,DIM_X,DIM_Y,DIM_Z,DIM_TIME,DIM_VEC} ;
    dim_info():length(0),step(0),start(0),have_dir_cos(false)
    {
      dir_cos[0]=dir_cos[1]=dir_cos[2]=0.0;
    }
    dim_info(int l, double sta,double spa,dimensions d,bool hd=false);
    size_t length;
    double step,start;
    bool have_dir_cos;
    double dir_cos[3];
    std::string name;
    dimensions  dim;
  };
  
  //! collection of dimensions describing a minc file
  typedef std::vector<dim_info> minc_info;
  
  //! minc file rw base class
  class minc_1_base
  {
  protected:
    int _slab_len;
    int _icvid;
    std::vector<long> _cur,_slab;
    size_t  _slice_dimensions;
    bool _last;
    bool _positive_directions;
    nc_type _datatype;
    nc_type _io_datatype;
    char _dimension_names[MAX_VAR_DIMS][MAX_NC_NAME];
    std::vector<double> _dir_cos;
    long vcount[MAX_VAR_DIMS];
    std::vector<double> _world_matrix;
    std::vector<int>    _voxel_matrix;
    int _ndims, mdims[MAX_VAR_DIMS];
    int _is_signed;
    int _mincid, _imgid;
    int _icmax,_icmin;
    double _image_range[2];
    std::vector<long> _dims;
    std::vector<int> _map_to_std;
    minc_info _info;
    bool _minc2;
  public:
    
    //! get the minc handle
    int mincid(void) const  //this is not really a const ?
    {
      return _mincid;
    }
    
    //! get the data type id (NC_BYTE,NC_INT etc)
    nc_type datatype(void) const
    {
      return _datatype;
    }
    
    //! byte size of the volume elements
    unsigned int element_size(void) const
    {
      switch(_io_datatype)
      {
        case NC_FLOAT: return sizeof(float);
        case NC_DOUBLE: return sizeof(double);
        case NC_SHORT: return sizeof(short);
        case NC_BYTE: return sizeof(char);
        default:return 0;//maybe throw exception here?
      }
    }
    
    //! is data stored in signed format
    bool is_signed(void) const
    {
      return _is_signed;
    }
  
    //! constructor
	  minc_1_base();

    //! destructor, closes minc file
    virtual ~minc_1_base();
    
    //! close the minc file
    virtual void close();

    //! is last slice was read?
    bool last(void) const 
    {
      return _last;
    }
    
    //! go to the beginning of file
    void begin(void)
    {
      fill(_cur.begin(),_cur.end(),0);
      _last=false;
    }
    
    //! advance to next slice 
    bool next_slice(void)
    {
      if(_last) return !_last;
        
      for(int i=_ndims-_slice_dimensions-1;i>=0;i--)
      {
        _cur[i]++;
        if(_cur[i]<static_cast<long>(_info[i].length))
          break;
        if(!i)
          _last=true;
        else 
          _cur[i]=0;
      }
      return !_last;
    }
    
    //! slice length in elements
    int slice_len(void) const
    {
      return _slab_len;
    }
    
    //! number of dimensions
    int dim_no(void) const
    {
      return _ndims;
    }
    
    //! get the dimension information
    const dim_info& dim(unsigned int n) const
    {
      if(n>=static_cast<unsigned int>(_ndims)) 
	REPORT_ERROR("Dimension is not defined");
      return _info[n];
    }
    
    //! get the pointer to the dimension description array
    const minc_info& info(void) const
    {
      return _info;
    }
    
    //! get the number of dimensions in one slice
    int slice_dimensions(void) const
    {
      return _slice_dimensions;
    }
    
    //! get the current slice index
    const std::vector<long> & current_slice(void) const
    {
      return _cur;
    }
    
    //! get the normalized dimensions sizes 
    //! ( 0 - vector_dimension, 1 - x, 2- y , 3 -z , 4 - time)
    int ndim(int i) const
    {
      int j=_map_to_std[i];
      if(j>=0) return _info[j].length;
      return 0;
    }
    //! get normalized dimension start coordinate (see ndim)
    double nstart(int i) const
    {
      int j=_map_to_std[i];
      if(j>=0) return _info[j].start;
      return 0.0;
    }
    
    //! get normalized dimension spacing  (see ndim)
    double nspacing(int i) const
    {
      int j=_map_to_std[i];
      if(j>=0) return _info[j].step;
      return 0.0;
    }
    
    //! get normalized dimension direction cosine component  (see ndim)
    double ndir_cos(int i,int j) const
    {
      int k=_map_to_std[i];
      if(k>=0) return _info[k].dir_cos[j];
      return 0.0;
    }
    
    //! check if a normalized dimension has direction cosine information
    bool have_dir_cos(int i) const
    {
      int k=_map_to_std[i];
      if(k>=0) return _info[k].have_dir_cos;
      return false;
    }
     
    //! map file dimensions into normalized dimensions
    int map_space(int i)
    {
      return _map_to_std[i];
    }
    
    //metadate info handling function:
    //! read the minc history (:history attribute)
    std::string history(void) const;
    
    //! retrive var id, if it exists, otherwise return MI_ERROR
    int var_id(const char *var_name) const;
    
    //! get variable length
    long var_length(const char *var_name) const;
    
    //! get variable length
    long var_length(int var_id) const;
    
    //! read the number of variables
    int var_number(void) const;
    
    //! get the variable name number no
    std::string var_name(int no) const;
    
    //! get the number of attributes associated with variable
    int att_number(const char *var_name) const;
    
    //! get the number of attributes associated with variable
    int att_number(int var_no) const;
    
    //! get the attribute name, given the number
    std::string att_name(const char *var_name,int no) const;
    //! get the attribute name, given the number
    std::string att_name(int varid,int no) const;
    
    //! get the string attribute value , given the name
    std::string att_value_string(const char *var_name,const char *att_name) const;
    //! get the string attribute value , given variable id
    std::string att_value_string(int varid,const char *att_name) const;
    
    //! get the double attribute value , given the name
    std::vector<double> att_value_double(const char *var_name,const char *att_name) const;
    //! get the int attribute value , given the name
    std::vector<int> att_value_int(const char *var_name,const char *att_name) const;
    //! get the short attribute value , given the variable id
    std::vector<short> att_value_short(const char *var_name,const char *att_name) const;
    //! get the byte attribute value , given the variable id
    std::vector<unsigned char> att_value_byte(const char *var_name,const char *att_name) const;
    
    //! get the double attribute value , given the variable id
    std::vector<double> att_value_double(int varid,const char *att_name) const;
    //! get the int attribute value , given the variable id
    std::vector<int> att_value_int(int varid,const char *att_name) const;
    //! get the short attribute value , given the variable id
    std::vector<short> att_value_short(int varid,const char *att_name) const;
    //! get the byte attribute value , given the variable id
    std::vector<unsigned char> att_value_byte(int varid,const char *att_name) const;
    
    //! enquire about attribute data type
    nc_type att_type(const char *var_name,const char *att_name) const;
    //! enquire about attribute data type
    nc_type att_type(int varid,const char *att_name) const;

    //! enquire about attribute length
    int att_length(const char *var_name,const char *att_name) const;
    //! enquire about attribute length
    int att_length(int varid,const char *att_name) const;


    //! return var_id for the given name (create one, if it doesn't exists)
    int create_var_id(const char *varname);
    
    void insert(const char *varname,const char *attname,double val);
    void insert(const char *varname,const char *attname,const char* val);
    void insert(const char *varname,const char *attname,const std::vector<double> &val);
    void insert(const char *varname,const char *attname,const std::vector<int> &val);
    void insert(const char *varname,const char *attname,const std::vector<short> &val);
    void insert(const char *varname,const char *attname,const std::vector<unsigned char> &val);
    
    
    //! check if the file in MINC2 format
    bool is_minc2(void) const
    {
      return _minc2;
    }
    
  };
  
  //! minc file reader
  class minc_1_reader:public minc_1_base
  {
    protected:
      bool _metadate_only;
      std::string _tempfile;
      bool _have_temp_file;
      bool _read_prepared;
      void _setup_dimensions(void);

    public:
    //! copy constructor
    minc_1_reader(const minc_1_reader&);
    
    //! default constructor
    minc_1_reader();
    
    //! destructor
    virtual ~minc_1_reader();
    //! open a minc file
    void open(const char *path,bool positive_directions=false,bool metadate_only=false,bool rw=false);
    
    //! read single slice
    void read(void* slice);
    //! setup reading in float format
    void setup_read_float(void);
    //! setup reading in double format
    void setup_read_double(void);
    //! setup reading in signed short format
    void setup_read_short(bool normalized=false);
    //! setup reading in unsigned short format
    void setup_read_ushort(bool normalized=false);
    //! setup reading in byte format
    void setup_read_byte(bool normalized=false);
    //! setup reading in int format
    void setup_read_int(bool normalized=false);
    //! setup reading in unsigned int format
    void setup_read_uint(bool normalized=false);
  };
  
  //! minc file writer
  class minc_1_writer:public minc_1_base
  {
    protected:
      bool _set_image_range;
      bool _set_slice_range;
      bool _calc_min_max;
      bool _write_prepared;
    public:
      void open(const char *path,const minc_info& inf,int slice_dimensions,nc_type datatype,int __signed=0);
      void open(const char *path,const minc_1_base& imitate);
      void open(const char *path,const char *imitate_file);
    
      void setup_write_float(void);
      void setup_write_double(void);
      void setup_write_short(bool normalize=false);
      void setup_write_ushort(bool normalize=false);
      void setup_write_byte(bool normalize=false);
      void setup_write_int(bool normalize=false);
      void setup_write_uint(bool normalize=false);
    
      //! copy header from another minc file
      void copy_headers(const minc_1_base& src);
    
      //! append a line into minc history
      void append_history(const char *append_history);
      
      
      //! constructor
      minc_1_writer();
      
      minc_1_writer(const minc_1_writer&);
      
      //! destructor
      virtual ~minc_1_writer();
      
      //!write a single slice, size of the buffer should be more or equall to slab_len
      void write(void* slice);
  };
};
#endif //__PRIMITIVE_MINC_IO__
