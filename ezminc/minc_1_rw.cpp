/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@DESCRIPTION: Primitive" interface to minc files, using minctoraw and rawtominc programs
              does not require linking with minc2 library.
              Created during the days when minc2 didn't compile on 64bit architecture
@COPYRIGHT  :
              Copyright 2007 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
//minc stuff
#include <math.h>
#include <limits.h>
#include "minc_1_rw.h"

namespace minc
{
  dim_info::dim_info(int l, double sta,
                     double spa,dimensions d,
                     bool hd):
      length(l),start(sta),step(spa),dim(d),have_dir_cos(hd)
  {
    switch(dim)
    {
      case dim_info::DIM_X:name=MIxspace;break;
      case dim_info::DIM_Y:name=MIyspace;break;
      case dim_info::DIM_Z:name=MIzspace;break;
      case dim_info::DIM_TIME:name=MItime;break;
      case dim_info::DIM_VEC:name=MIvector_dimension;break;
      default: REPORT_ERROR("Unknown Dimension!");
    }
  }
  
  minc_1_base::minc_1_base():
    _dims(3,0),
    _map_to_std(5,-1),
    _mincid(MI_ERROR),
    _imgid(MI_ERROR),
    _icvid(MI_ERROR),
    _cur(MAX_VAR_DIMS,0),
    _slab(MAX_VAR_DIMS,1),
    _last(false),
    _datatype(MI_ORIGINAL_TYPE),
    _io_datatype(MI_ORIGINAL_TYPE),
    _slab_len(0),
    _positive_directions(true),
    _minc2(false)
  {
    _icvid=miicv_create();
  }

  //! destructor, closes minc file
  minc_1_base::~minc_1_base()
  {
    close();
  }
  
  void minc_1_base::close(void)
  {
    if(_icvid!=MI_ERROR)
    {
      miicv_free(_icvid);
      _icvid=MI_ERROR;
    }
    if(_mincid!=MI_ERROR) miclose(_mincid);
    _mincid=MI_ERROR;
  }
  
  
  std::string minc_1_base::history(void) const
  {
    nc_type datatype;
    int att_length;
#ifndef WIN32
    int op=ncopts;
#endif //WIN32
    //ncopts=0;
    if ((ncattinq(_mincid, NC_GLOBAL, MIhistory, &datatype,&att_length) == MI_ERROR) ||
        (datatype != NC_CHAR))
    {
      //ncopts=op;
      return "";
    }
    char* str = new char[att_length+1];
    str[0] = '\0';
    miattgetstr(_mincid, NC_GLOBAL, (char*)MIhistory, att_length,str);
    //ncopts=op;
    std::string r(str);
    delete [] str;
    return r;
  }
  
  //code from mincinfo
  int minc_1_base::var_number(void) const
  {
    int nvars;
    if(ncinquire(_mincid, NULL, &nvars, NULL, NULL)!=MI_ERROR)
      return nvars;
    return 0;
  }
  
  std::string minc_1_base::var_name(int no) const
  {
    char name[MAX_NC_NAME];
    if(ncvarinq(_mincid, no, name, NULL, NULL, NULL, NULL)!=MI_ERROR)
      return name;
  }
  
  int minc_1_base::att_number(const char *var_name) const
  {
    int varid;
    int natts;
    if (*var_name=='\0') {
        varid = NC_GLOBAL;
    } else {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return 0;
    }
    return att_number(varid);
  }
  
  int minc_1_base::var_id(const char *var_name) const
  {
    return ncvarid(_mincid, var_name);
  }

  long minc_1_base::var_length(const char *var_name) const
  {
    int varid=var_id(var_name);
    if(varid!=MI_ERROR)
      return var_length(varid);
    else
      return 0;
  }
    
  long minc_1_base::var_length(int var_id) const
  {
    int vardims;
    
    if(ncvarinq(_mincid, var_id, NULL, NULL, &vardims, NULL, NULL)!=MI_ERROR)
    {
      if(vardims==0) return 1;
      int *dims=new int[vardims];
      if(ncvarinq(_mincid, var_id, NULL, NULL, NULL, dims, NULL)!=MI_ERROR)
      {
        long varlength=1;
        if(ncdiminq(_mincid,dims[0],NULL,&varlength)!=MI_ERROR)
        {
          delete[] dims;
          return varlength;
        }
        delete[] dims;
        return 1;
      } else {
        delete[] dims;
        return 1;
      }
    }
    return 0;
  }

  
  //! get the number of attributes associated with variable
  int minc_1_base::att_number(int var_no) const
  {
    int natts;
    if(ncvarinq(_mincid, var_no, NULL, NULL, NULL, NULL, &natts)!=MI_ERROR)
      return natts;
    return 0;
  }
  
  
  std::string minc_1_base::att_name(const char *var_name,int no) const
  {
    int varid;
    int attid;
    char name[MAX_NC_NAME];
    if (*var_name=='\0') 
        varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return "";
    }
    return att_name(varid,no);
  }
  
  std::string minc_1_base::att_name(int varid,int no) const
  {
    int attid;
    char name[MAX_NC_NAME];
    if(ncattname(_mincid, varid, no, name)==MI_ERROR)
      return "";
    
    return name;
  }
  
  std::string minc_1_base::att_value_string(const char *var_name,const char *att_name) const
  {
    int varid;
    int attid;
    char name[MAX_NC_NAME];
    if (*var_name=='\0') 
        varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return "";
    }
    return att_value_string(varid,att_name);
  }
  
  std::string minc_1_base::att_value_string(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    //TODO: make this handle other (double?) data types correctly
    if ((ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR) ||
        (datatype != NC_CHAR))
    {
      //ncopts=op;
      return "";
    }
    char* str = new char[att_length+1];
    str[0] = '\0';
    miattgetstr(_mincid, varid, (char *)att_name, att_length, str);
    //ncopts=op;
    std::string r(str);
    delete [] str;
    return r;
  }
  
  std::vector<double> minc_1_base::att_value_double(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
        varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return std::vector<double>(0);
    }
    return att_value_double(varid,att_name);
  }
  
  std::vector<short> minc_1_base::att_value_short(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
      varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return std::vector<short>(0);
    }
    return att_value_short(varid,att_name);
  }
  
  std::vector<unsigned char> minc_1_base::att_value_byte(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
      varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return std::vector<unsigned char>(0);
    }
    return att_value_byte(varid,att_name);    
  }
  
  
  std::vector<int> minc_1_base::att_value_int(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
      varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return std::vector<int>(0);
    }
    return att_value_int(varid,att_name);
  }
  
  std::vector<int> minc_1_base::att_value_int(int varid,const char *att_name) const 
  {
    int att_length;
    nc_type datatype;
    
    if ((ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR) ||
         (datatype != NC_INT))
    {
      //ncopts=op;
      return std::vector<int>(0);
    }
    std::vector<int> r(att_length);
    miattget(_mincid, varid, (char*)att_name, NC_INT, att_length,&r[0], NULL) ;
    //ncopts=op;
    return r;
  }

  
  std::vector<double> minc_1_base::att_value_double(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    //TODO: make this handle other (double?) data types correctly
    if ((ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR) ||
        (datatype != NC_DOUBLE))
    {
      //ncopts=op;
      return std::vector<double>(0);
    }
    std::vector<double> r(att_length);
    miattget(_mincid, varid, (char*)att_name, NC_DOUBLE, att_length,&r[0], NULL) ;
    //ncopts=op;
    return r;
  }
  
  std::vector<short> minc_1_base::att_value_short(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    //TODO: make this handle other (double?) data types correctly
    if ((ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR) ||
         (datatype != NC_SHORT))
    {
      //ncopts=op;
      return std::vector<short>(0);
    }
    std::vector<short> r(att_length);
    miattget(_mincid, varid, (char*)att_name, NC_SHORT, att_length,&r[0], NULL) ;
    //ncopts=op;
    return r;
  }
  
  std::vector<unsigned char> minc_1_base::att_value_byte(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    //TODO: make this handle other (double?) data types correctly
    if ((ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR) ||
         (datatype != NC_BYTE))
    {
      //ncopts=op;
      return std::vector<unsigned char>(0);
    }
    std::vector<unsigned char> r(att_length);
    miattget(_mincid, varid, (char*)att_name, NC_BYTE, att_length,&r[0], NULL) ;
    //ncopts=op;
    return r;
  }
  
  
  nc_type minc_1_base::att_type(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
        varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return MI_ORIGINAL_TYPE;
    }
    return att_type(varid,att_name);
  }
  
  nc_type minc_1_base::att_type(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    if(ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR)
      return MI_ORIGINAL_TYPE;
    return datatype;
  }
  

  int minc_1_base::att_length(const char *var_name,const char *att_name) const
  {
    int varid;
    if (*var_name=='\0') 
        varid = NC_GLOBAL;
    else 
    {
      if((varid = ncvarid(_mincid, var_name))==MI_ERROR)
        return 0;
    }
    return att_length(varid,att_name);
  }
  
  int minc_1_base::att_length(int varid,const char *att_name) const
  {
    int att_length;
    nc_type datatype;
    
    if(ncattinq(_mincid, varid, (char *)att_name, &datatype,&att_length) == MI_ERROR)
      return 0;
    
    return att_length;
  }
  
  
  minc_1_reader::minc_1_reader(const minc_1_reader&):_metadate_only(false),_have_temp_file(false),_read_prepared(false)
  {
  }
  
  minc_1_reader::minc_1_reader():_metadate_only(false),_have_temp_file(false),_read_prepared(false)
  {
  }
  
  
  //based on the code from mincextract
  void minc_1_reader::open(const char *path,bool positive_directions/*=true*/,bool metadate_only/*=false*/,bool rw/*=false*/)
  {
#ifndef WIN32
    ncopts = 0;
#endif 
    _metadate_only=metadate_only;
    _read_prepared=false;
    int element_size;
    int idim;
    int nstart, ncount;
    _positive_directions=positive_directions;
    //ncopts = 0;

    // Open the file 

    // Expand file 
    if(_metadate_only)
    { 
      int created_tempfile;
      char * tempfile = miexpand_file((char*)path, NULL, true, &created_tempfile);
      if (tempfile == NULL) REPORT_ERROR("Error expanding minc file");
      _tempfile=tempfile;
      path=_tempfile.c_str();
      _have_temp_file=created_tempfile;
    }
    _mincid = miopen((char*)path, rw?NC_WRITE:NC_NOWRITE);

    if(_mincid == MI_ERROR) REPORT_ERROR("Can't open minc file for reading!");
#ifdef MINC2
   if (MI2_ISH5OBJ(_mincid)) {
     _minc2 = true;
   }
#endif

    /* Inquire about the image variable */
    _imgid = ncvarid(_mincid, MIimage);
    if(_imgid == MI_ERROR) REPORT_ERROR("Can't get Image ID");
    ncvarinq(_mincid, _imgid, NULL, NULL, &_ndims, mdims, NULL);
    //get image data type... not used for now
    miget_datatype(_mincid, _imgid, &_datatype, &_is_signed);
    //dir_cos.SetIdentity();
    int dim_cnt=0;
    miget_image_range(_mincid, _image_range);
    //go through dimensions , calculating parameters for reshaping into ZYX array if needed
    _info.resize(_ndims);
    _world_matrix.resize(_ndims*4,0.0);
    _voxel_matrix.resize(_ndims*4,0);
    //_dir_cos.resize(_ndims*_ndims,0.0);
    
    for(int i=_ndims-1;i>=0;i--) 
    {
      //_world_matrix[i*(_ndims+1)]=1.0;
      //_voxel_matrix[i*(_ndims+1)]=1.0;
      char dimname[MAX_NC_NAME];
      long dimlength;
      //get dimensions info
      ncdiminq(_mincid, mdims[i], dimname, &dimlength);
      _info[i].name=dimname;
      _info[i].length=dimlength;
      _info[i].have_dir_cos=false;
      int axis=-1;
      unsigned int sz=0;
      
      if(!strcmp(dimname,MIxspace))
      { 
        _dims[0]=dimlength;axis=0;
        _info[i].dim=dim_info::DIM_X;
        _map_to_std[1]=i;
      } else if(!strcmp(dimname,MIyspace)) { 
        _dims[1]=dimlength;axis=1;
        _info[i].dim=dim_info::DIM_Y;
        _map_to_std[2]=i;
      } else if(!strcmp(dimname,MIzspace)) { 
        _dims[2]=dimlength;axis=2;
        _info[i].dim=dim_info::DIM_Z;
        _map_to_std[3]=i;
      } else if(!strcmp(dimname,MIvector_dimension)) { 
         axis=-1;
        _info[i].dim=dim_info::DIM_VEC;
        _map_to_std[0]=i;
      } else if(!strcmp(dimname,MItime)) { 
         axis=3;
        _info[i].dim=dim_info::DIM_TIME;
        _map_to_std[4]=i;
      } else  {
        REPORT_ERROR ("Unknown dimension");
        _info[i].dim=dim_info::DIM_UNKNOWN;
      }
      
      if(_info[i].dim!=dim_info::DIM_VEC)
      {
        //ncopts = 0;
        int dimid = ncvarid(_mincid, dimname);
        //ncopts = NC_VERBOSE | NC_FATAL;
        if (dimid == MI_ERROR) continue;
               
        // Get dimension attributes
        //ncopts = 0;
        miattget1(_mincid, dimid, (char*)MIstep, NC_DOUBLE, &_info[i].step);
        if(_info[i].step == 0.0)
           _info[i].step = 1.0;
        miattget1(_mincid, dimid, (char*)MIstart, NC_DOUBLE, &_info[i].start);
          
        if(_positive_directions && _info[i].step<0.0)
        {
          _info[i].start+=_info[i].step*(dimlength-1);
          _info[i].step=-_info[i].step;
        }
        
        if(miattget(_mincid, dimid, (char*)MIdirection_cosines, NC_DOUBLE, 3, &_info[i].dir_cos[0], NULL)!= MI_ERROR)
        {
          _info[i].have_dir_cos=true;
          
          /* Normalize the direction cosine */
          double len=sqrt(_info[i].dir_cos[0]*_info[i].dir_cos[0]+
                          _info[i].dir_cos[1]*_info[i].dir_cos[1]+
                          _info[i].dir_cos[2]*_info[i].dir_cos[2]);
          
          if(len>1e-6 && fabs(len-1.0)>1e-6) //TODO: use some epsiolon here?
          {
            for(int a=0;a<3;a++)
              _info[i].dir_cos[a]/=len;
          }
          
        } 
        // fill voxel matrix
        _voxel_matrix[i*4+axis]=1;
      } else { //vectors don't have spatial component!
        _info[i].start=0;
        _info[i].step=0.0;
        _info[i].dir_cos[0]=_info[i].dir_cos[1]=_info[i].dir_cos[2]=0.0;
        _info[i].have_dir_cos=false;
      }
      
      //fill world matrix
      for(int a=0;a<3;a++)
        _world_matrix[i*4+a]=_info[i].dir_cos[a]*_info[i].step;
      if(axis==3) //time
        _world_matrix[i*4+3]=_info[i].step;
      else
        _world_matrix[i*4+3]=0.0;
    }
    //ncopts = NC_VERBOSE | NC_FATAL;
    
    // now let's find out the slice dimensions
    int idmax = ncvarid(_mincid, MIimagemax);
    _slice_dimensions=0;
    if(idmax != MI_ERROR) 
    {
      int nmax_dims;
      int mmax_dims[MAX_VAR_DIMS];
      ncvarinq(_mincid, _imgid, NULL, NULL, &nmax_dims, mmax_dims, NULL);
      if(nmax_dims>0)
        _slice_dimensions=_ndims-nmax_dims;
    } 
    
    if(_slice_dimensions<=0)
    {
      if(_info[_ndims-1].dim==dim_info::DIM_VEC || _info[_ndims-1].dim==dim_info::DIM_TIME) 
        _slice_dimensions=std::min(_ndims,3);
      else 
        _slice_dimensions=std::min(_ndims,2);
    }
    std::fill(_slab.begin(),_slab.end(),1);
    _slab_len=1;
    for(int i=0;i<_slice_dimensions;i++)
    {
      _slab[_ndims-i-1]=_info[_ndims-i-1].length;
      _slab_len*=_info[_ndims-i-1].length;
    }
  }
  
  minc_1_reader::~minc_1_reader()
  {
    if(_have_temp_file)
      remove(_tempfile.c_str());
  }

  minc_1_writer::minc_1_writer():
      _set_image_range(false),_set_slice_range(false),
      _calc_min_max(true),_write_prepared(false)
  {
  }

  minc_1_writer::minc_1_writer(const minc_1_writer&):
      _set_image_range(false),_set_slice_range(false),
      _calc_min_max(true),_write_prepared(false)
  {
  }
  
  
  void minc_1_writer::open(const char *path,const minc_info& inf,int slice_dimensions,nc_type datatype,int _s)
  {
#ifndef WIN32
    ncopts = 0;
#endif
	  _info=inf;
    //int  mdims[MAX_VAR_DIMS];
    double vrange[2];
    _write_prepared=false;
    
    _mincid = micreate((char*)path, NC_CLOBBER/*|MI2_CREATE_V2*/); //TODO: add environment variable checking
#ifdef MINC2
    if (MI2_ISH5OBJ(_mincid)) { //micreate might create MINC2 file if environment variable is set
      _minc2 = true;
    }
#endif
    _ndims=_info.size();
    _datatype=datatype;
    _slice_dimensions=slice_dimensions;
    _is_signed=_s;
    fill(_map_to_std.begin(),_map_to_std.end(),-1);
    for(int i=_ndims-1;i>=0;i--)
    {
      //just a precaution
      switch(_info[i].dim)
      {
        case dim_info::DIM_X:_info[i].name=MIxspace;_map_to_std[1]=i;break;
        case dim_info::DIM_Y:_info[i].name=MIyspace;_map_to_std[2]=i;break;
        case dim_info::DIM_Z:_info[i].name=MIzspace;_map_to_std[3]=i;break;
        case dim_info::DIM_TIME:_info[i].name=MItime;_map_to_std[4]=i;break;
        default:
        case dim_info::DIM_VEC:_info[i].name=MIvector_dimension;_map_to_std[0]=i;break;
        //default: REPORT_ERROR("Unknown Dimension!");
      }
      mdims[i]=ncdimdef(_mincid, _info[i].name.c_str(), _info[i].length);
      if(_info[i].dim!=dim_info::DIM_VEC)
      {
        int dimid=micreate_std_variable(_mincid,(char*)_info[i].name.c_str(),NC_INT, 0, NULL);
        miattputdbl(_mincid, dimid, (char*)MIstep,_info[i].step);
        miattputdbl(_mincid, dimid, (char*)MIstart,_info[i].start);
        
        if(_info[i].have_dir_cos)
          ncattput(_mincid, dimid, (char*)MIdirection_cosines,NC_DOUBLE, 3, _info[i].dir_cos);
      }
    }
    _slab_len=1;
    for(int i=0;i<_slice_dimensions;i++)
    {
      _slab[_ndims-i-1]=_info[_ndims-i-1].length;
      _slab_len*=_info[_ndims-i-1].length;
    }
    
    _icmax=_icmin=MI_ERROR;
    //ncopts = NC_OPTS_VAL;
    _imgid=micreate_std_variable(_mincid, (char*)MIimage, _datatype, _ndims, mdims);
    _image_range[0]=DBL_MAX;_image_range[1]=-DBL_MAX;
    
    switch(_datatype)
    {
      case NC_DOUBLE:
        vrange[0]=-DBL_MAX;vrange[1]=DBL_MAX;
        _is_signed=1;
        break;
      case NC_FLOAT:
        vrange[0]=-FLT_MAX;vrange[1]=FLT_MAX;
        _is_signed=1;
        break;
      case NC_SHORT:
        if(_is_signed)
        {
          vrange[0]=SHRT_MIN;
          vrange[1]=SHRT_MAX;
        } else {
          vrange[0]=0;vrange[1]=USHRT_MAX;
        }
        break;
      case NC_BYTE:
        if(_is_signed)
        {
          vrange[0]=-128;vrange[1]=127;
        } else {
          vrange[0]=0;vrange[1]=255;
        }
        break;
      case NC_INT:
        if(_is_signed)
        {
          vrange[0]=INT_MIN;vrange[1]=INT_MAX;
        }else{
          vrange[0]=0;vrange[1]=UINT_MAX;
        }
        break;
      default:break;
    };
    miattputstr(_mincid, _imgid, (char*)MIcomplete, (char*)MI_FALSE);
    miattputstr(_mincid, _imgid, (char*)MIsigntype, (char*)(_is_signed?MI_SIGNED:MI_UNSIGNED));
    ncattput(_mincid, _imgid, (char*)MIvalid_range, NC_DOUBLE, 2, vrange);
    miset_valid_range(_mincid, _imgid, vrange);
  }
  
  void minc_1_writer::open(const char *path,const minc_1_base& imitate)
  {
    
    open(path,imitate.info(),imitate.slice_dimensions(), 
         imitate.datatype(),imitate.is_signed());
    
    copy_headers(imitate);
  }
  
  void minc_1_writer::open(const char *path,const char *imitate_file)
  {
    minc_1_reader rdr;
    //open minc file in metadate mode
    rdr.open(imitate_file,false,true);
    open(path,rdr);
    //copy_headers(rdr);
  }
   
  void minc_1_writer::setup_write_float()
  {
    _image_range[0]=DBL_MAX;_image_range[1]=-DBL_MAX;
    
    switch(_datatype)
    {
      case NC_DOUBLE:
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
      
        _set_image_range=true;
        _set_slice_range=false;
        break;
      
      case NC_FLOAT:
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
      
        _set_image_range=true;
        _set_slice_range=false;
        break;
      
      case NC_SHORT:
        
          _set_image_range=false;
          _set_slice_range=true;
        
          _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
          _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      
      case NC_BYTE:
        _set_image_range=false;
        _set_slice_range=true;
      
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      case NC_INT:
        _set_image_range=false;
        _set_slice_range=true;
      
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      
      default:
        break;
    };
    ncendef(_mincid);
    
    if(_datatype==NC_DOUBLE || _datatype==NC_FLOAT)
    {
      miicv_setstr(_icvid, MI_ICV_SIGN,   (char*)MI_SIGNED);
      miicv_setint(_icvid, MI_ICV_TYPE,    NC_FLOAT);
      miicv_setint(_icvid, MI_ICV_DO_NORM,    true);
      miicv_setint(_icvid, MI_ICV_USER_NORM, true);
      miicv_setdbl(_icvid, MI_ICV_VALID_MIN, -FLT_MAX);
      miicv_setdbl(_icvid, MI_ICV_VALID_MAX, FLT_MAX);
      _calc_min_max=true;
      
    } else { //do something smart here?
      miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);
      miicv_setint(_icvid, MI_ICV_TYPE, NC_FLOAT);
      miicv_setint(_icvid, MI_ICV_DO_NORM, false);
      //miicv_setint(_icvid, MI_ICV_USER_NORM, false);
      miicv_setdbl(_icvid, MI_ICV_VALID_MIN, -FLT_MAX);
      miicv_setdbl(_icvid, MI_ICV_VALID_MAX, FLT_MAX);
      _calc_min_max=true;
      
    }
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_FLOAT;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_double()
  {
    _image_range[0]=DBL_MAX;_image_range[1]=-DBL_MAX;
    
    switch(_datatype)
    {
      case NC_DOUBLE:
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
      
        _set_image_range=true;
        _set_slice_range=false;
        break;
      
      case NC_FLOAT:
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
      
        _set_image_range=true;
        _set_slice_range=false;
        break;
      
      case NC_SHORT:
        
          _set_image_range=false;
          _set_slice_range=true;
        
          _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
          _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      
      case NC_BYTE:
        _set_image_range=false;
        _set_slice_range=true;
      
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      case NC_INT:
        _set_image_range=false;
        _set_slice_range=true;
      
        _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, _ndims-_slice_dimensions, mdims);
        break;
      
      default:
        break;
    };
    ncendef(_mincid);
    
    if(_datatype==NC_DOUBLE)
    {
      miicv_setstr(_icvid, MI_ICV_SIGN,   (char*)MI_SIGNED);
      miicv_setint(_icvid, MI_ICV_TYPE,    NC_DOUBLE);
      miicv_setint(_icvid, MI_ICV_DO_NORM,    true);
      miicv_setint(_icvid, MI_ICV_USER_NORM, true);
      miicv_setdbl(_icvid, MI_ICV_VALID_MIN, -DBL_MAX);
      miicv_setdbl(_icvid, MI_ICV_VALID_MAX, DBL_MAX);
      _calc_min_max=true;
      
    } else if(_datatype==NC_FLOAT)  {
      miicv_setstr(_icvid, MI_ICV_SIGN,   (char*)MI_SIGNED);
      miicv_setint(_icvid, MI_ICV_TYPE,    NC_DOUBLE);
      miicv_setint(_icvid, MI_ICV_DO_NORM,    true);
      miicv_setint(_icvid, MI_ICV_USER_NORM, true);
      miicv_setdbl(_icvid, MI_ICV_VALID_MIN, -DBL_MAX);
      miicv_setdbl(_icvid, MI_ICV_VALID_MAX, DBL_MAX);
      _calc_min_max=true;
      
    } else { //do something smart here?
      miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);
      miicv_setint(_icvid, MI_ICV_TYPE, NC_DOUBLE);
      miicv_setint(_icvid, MI_ICV_DO_NORM, false);
      //miicv_setint(_icvid, MI_ICV_USER_NORM, false);
      miicv_setdbl(_icvid, MI_ICV_VALID_MIN, -DBL_MAX);
      miicv_setdbl(_icvid, MI_ICV_VALID_MAX, DBL_MAX);
      _calc_min_max=true;
      
    }
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_DOUBLE;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_short(bool n)
  {
    _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
    _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
    _set_image_range=true;
    _set_slice_range=false;
    
    ncendef(_mincid);
    
    miicv_setint(_icvid, MI_ICV_TYPE, NC_SHORT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);
    //miicv_setstr(_icvid, MI_ICV_SIGN, true);    
    /* Set range of values */ //TODO: set this to something sensible?
    miicv_setint(_icvid, MI_ICV_VALID_MIN, SHRT_MIN);
    miicv_setint(_icvid, MI_ICV_VALID_MAX, SHRT_MAX);

    /* No normalization so that pixels are scaled to the slice */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    miicv_setint(_icvid, MI_ICV_DO_RANGE, false);
    
    miicv_attach(_icvid, _mincid, _imgid);
    
    _io_datatype=NC_SHORT;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_ushort(bool n)
  {
    _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
    _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
    _set_image_range=true;
    _set_slice_range=false;
    
    ncendef(_mincid);
    miicv_setint(_icvid, MI_ICV_TYPE, NC_SHORT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);
    miicv_setstr(_icvid, MI_ICV_SIGN, false); 
    
    /* Set range of values */ //TODO: set this to something sensible?
    miicv_setint(_icvid, MI_ICV_VALID_MIN, 0);
    miicv_setint(_icvid, MI_ICV_VALID_MAX, USHRT_MAX);

    /* No normalization so that pixels are scaled to the slice */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    miicv_setint(_icvid, MI_ICV_DO_RANGE, false);
    
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_SHORT;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_byte(bool n)
  {
    _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
    _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
    _set_image_range=true;
    _set_slice_range=false;
    
    ncendef(_mincid);
    miicv_setint(_icvid, MI_ICV_TYPE, NC_BYTE);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);

    /* Set range of values */ //TODO: set this to something sensible?
    miicv_setint(_icvid, MI_ICV_VALID_MIN, 0);
    miicv_setint(_icvid, MI_ICV_VALID_MAX, UCHAR_MAX);

    /* No normalization so that pixels are scaled to the slice */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    miicv_setint(_icvid, MI_ICV_DO_RANGE, false);
    
    miicv_attach(_icvid, _mincid, _imgid);
    
    _io_datatype=NC_BYTE;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_int(bool n)
  {
    _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
    _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
    _set_image_range=true;
    _set_slice_range=false;
    
    ncendef(_mincid);
    miicv_setint(_icvid, MI_ICV_TYPE, NC_INT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);

    /* Set range of values */ //TODO: set this to something sensible?
    miicv_setint(_icvid, MI_ICV_VALID_MIN, INT_MIN);
    miicv_setint(_icvid, MI_ICV_VALID_MAX, INT_MAX);

    /* No normalization so that pixels are scaled to the slice */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    miicv_setint(_icvid, MI_ICV_DO_RANGE, false);
    
    miicv_attach(_icvid, _mincid, _imgid);

    
    _io_datatype=NC_INT;
    _write_prepared=true;
  }
  
  void minc_1_writer::setup_write_uint(bool n)
  {
    _icmax=micreate_std_variable(_mincid, (char*)MIimagemax, NC_DOUBLE, 0, NULL);
    _icmin=micreate_std_variable(_mincid, (char*)MIimagemin, NC_DOUBLE, 0, NULL);
    _set_image_range=true;
    _set_slice_range=false;
    
    ncendef(_mincid);
    miicv_setint(_icvid, MI_ICV_TYPE, NC_INT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);

    /* Set range of values */ //TODO: set this to something sensible?
    miicv_setint(_icvid, MI_ICV_VALID_MIN, 0);
    miicv_setint(_icvid, MI_ICV_VALID_MAX, UINT_MAX);

    /* No normalization so that pixels are scaled to the slice */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    miicv_setint(_icvid, MI_ICV_DO_RANGE, false);
    
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_INT;
    _write_prepared=true;
  }
  
  void minc_1_reader::_setup_dimensions(void)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    if(_positive_directions)
    {
      /* We want to ensure that images have X, Y and Z dimensions in the
      positive direction, giving patient left on left and for drawing from
      bottom up. If we wanted patient right on left and drawing from
      top down, we would set to MI_ICV_NEGATIVE. */
      
      miicv_setint(_icvid, MI_ICV_DO_DIM_CONV, true);
      //TODO: make sure to change only x,y,z conversions here
      //miicv_setint(_icvid, MI_ICV_XDIM_DIR, 3);
      //we want to convert only X,Y,Z dimensions if they are present
      int num=(_map_to_std[1]>=0?1:0)+(_map_to_std[2]>=0?1:0)+(_map_to_std[3]>=0?1:0);
      miicv_setint(_icvid, MI_ICV_NUM_IMGDIMS, num);
      
      if(_map_to_std[1]>=0) 
      {
        miicv_setint(_icvid, MI_ICV_DIM_SIZE+_map_to_std[1],-1);
        miicv_setint(_icvid, MI_ICV_XDIM_DIR, MI_ICV_POSITIVE);
      }
      
      if(_map_to_std[2]>=0) 
      {
        miicv_setint(_icvid, MI_ICV_DIM_SIZE+_map_to_std[2],-1);
        miicv_setint(_icvid, MI_ICV_YDIM_DIR, MI_ICV_POSITIVE);
      }
      
      if(_map_to_std[3]>=0) 
      {
        miicv_setint(_icvid, MI_ICV_DIM_SIZE+_map_to_std[3],-1);
        miicv_setint(_icvid, MI_ICV_ZDIM_DIR, MI_ICV_POSITIVE);
      }
    }
    miicv_setint(_icvid, MI_ICV_DO_SCALAR, false);
  }
  
  void minc_1_reader::setup_read_float(void)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    miicv_setint(_icvid, MI_ICV_TYPE, NC_FLOAT);
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    miicv_setint(_icvid, MI_ICV_USER_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_FLOAT;
    _read_prepared=true;
  }
  
  void minc_1_reader::setup_read_double(void)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    miicv_setint(_icvid, MI_ICV_TYPE, NC_DOUBLE);
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    miicv_setint(_icvid, MI_ICV_USER_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_DOUBLE;
    _read_prepared=true;
  }

  void minc_1_reader::setup_read_short(bool n)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    
    miicv_setint(_icvid, MI_ICV_TYPE, NC_SHORT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);
    /* Set range of values */
    miicv_setdbl(_icvid, MI_ICV_VALID_MIN, _image_range[0]);
    miicv_setdbl(_icvid, MI_ICV_VALID_MAX, _image_range[1]);

    /* Do normalization so that all pixels are on same scale */
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    //miicv_setint(_icvid, MI_ICV_USER_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_SHORT;
    _read_prepared=true;
  }
  
  void minc_1_reader::setup_read_ushort(bool n)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    
    miicv_setint(_icvid, MI_ICV_TYPE, NC_SHORT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);
    /* Set range of values */
    miicv_setdbl(_icvid, MI_ICV_VALID_MIN, _image_range[0]);
    miicv_setdbl(_icvid, MI_ICV_VALID_MAX, _image_range[1]);

    /* Do normalization so that all pixels are on same scale */
    miicv_setint(_icvid, MI_ICV_DO_NORM, false);
    //miicv_setint(_icvid, MI_ICV_USER_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_SHORT;
    _read_prepared=true;
  }
  
  void minc_1_reader::setup_read_byte(bool n)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    miicv_setint(_icvid, MI_ICV_TYPE, NC_BYTE);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);
    /* Set range of values */
    miicv_setdbl(_icvid, MI_ICV_VALID_MIN, _image_range[0]);
    miicv_setdbl(_icvid, MI_ICV_VALID_MAX, _image_range[1]);

   /* Do normalization so that all pixels are on same scale */
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_BYTE;
    _read_prepared=true;
  }
  
  void minc_1_reader::setup_read_int(bool n)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    miicv_setint(_icvid, MI_ICV_TYPE, NC_INT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_SIGNED);
    /* Set range of values */
    miicv_setdbl(_icvid, MI_ICV_VALID_MIN, _image_range[0]);
    miicv_setdbl(_icvid, MI_ICV_VALID_MAX, _image_range[1]);

   /* Do normalization so that all pixels are on same scale */
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_INT;
    _read_prepared=true;
  }
  
  void minc_1_reader::setup_read_uint(bool n)
  {
    if(_metadate_only)
      REPORT_ERROR("Minc file in metadate only mode!");
    miicv_setint(_icvid, MI_ICV_TYPE, NC_INT);
    miicv_setstr(_icvid, MI_ICV_SIGN, (char*)MI_UNSIGNED);
    /* Set range of values */
    miicv_setdbl(_icvid, MI_ICV_VALID_MIN, _image_range[0]);
    miicv_setdbl(_icvid, MI_ICV_VALID_MAX, _image_range[1]);

   /* Do normalization so that all pixels are on same scale */
    miicv_setint(_icvid, MI_ICV_DO_NORM, true);
    /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
    miicv_setint(_icvid, MI_ICV_DO_FILLVALUE, true);
    
    _setup_dimensions();
    miicv_attach(_icvid, _mincid, _imgid);
    _io_datatype=NC_INT;
    _read_prepared=true;
  }
  
  void minc_1_reader::read(void* buffer)
  {
    if(!_read_prepared)
      REPORT_ERROR("Not ready to read, use setup_read_XXXX");

    miicv_get(_icvid, &_cur[0], &_slab[0], buffer);
  }
  
  void minc_1_writer::write(void* buffer)
  {
    if(!_write_prepared)
      REPORT_ERROR("Not ready to write, use setup_write_XXXX");
    
    double r_min= DBL_MAX; //slab minimal value
    double r_max=-DBL_MAX; //slab maximal value
    //int irmin=0,irmax=0;
    if(_calc_min_max )
    {
      if(_io_datatype==NC_FLOAT)
      {// 
        float *tmp=(float*)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];//irmin=i;
          if(r_max<tmp[i]) r_max=tmp[i];//irmax=i;
        }
      } else if(_io_datatype==NC_DOUBLE) {
          double *tmp=(double*)buffer;
          for(int i=0;i<_slab_len;i++)
          {
            if(r_min>tmp[i]) r_min=tmp[i];//irmin=i;
            if(r_max<tmp[i]) r_max=tmp[i];//irmax=i;
          }
      } else if(_io_datatype==NC_SHORT && !_is_signed) {
        unsigned short *tmp=(unsigned short *)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];
          if(r_max<tmp[i]) r_max=tmp[i];
        }
      } else if(_io_datatype==NC_SHORT && _is_signed) {
        short *tmp=(short *)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];
          if(r_max<tmp[i]) r_max=tmp[i];
        }
      } else if(_io_datatype==NC_BYTE) {
        unsigned char *tmp=(unsigned char *)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];
          if(r_max<tmp[i]) r_max=tmp[i];
        }
      } else if(_io_datatype==NC_INT && _is_signed) {
        int *tmp=(int *)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];
          if(r_max<tmp[i]) r_max=tmp[i];
        }
      } else if(_io_datatype==NC_INT && !_is_signed) {
        unsigned int *tmp=(unsigned int *)buffer;
        for(int i=0;i<_slab_len;i++)
        {
          if(r_min>tmp[i]) r_min=tmp[i];
          if(r_max<tmp[i]) r_max=tmp[i];
        }
      }
      /*
      if(r_min<-10000||r_max>10000)
      {
        std::cerr<<r_min<<":"<<r_max<<" ";
        for(int i=0;i<4;i++)
          std::cerr<<_cur[i]<<",";
        std::cerr<<"  "<<r_min<<":"<<r_max;
        std::cerr<<" "<<irmin<<":"<<irmax<<"   "<<_slab_len;
        std::cerr<<std::endl;
      }*/
      
      if(_set_slice_range)
      {
        miicv_detach(_icvid);
        miicv_setdbl(_icvid, MI_ICV_VALID_MIN, r_min);
        miicv_setdbl(_icvid, MI_ICV_VALID_MAX, r_max);
        miicv_attach(_icvid, _mincid, _imgid);
      }
      
      if(_set_slice_range)
      {
        mivarput1(_mincid, _icmin, &_cur[0], NC_DOUBLE, NULL, &r_min);
        mivarput1(_mincid, _icmax, &_cur[0], NC_DOUBLE, NULL, &r_max);
      }
      
      if(_image_range[0]>r_min) _image_range[0]=r_min;
      if(_image_range[1]<r_max) _image_range[1]=r_max;
    }
    miicv_put(_icvid, &_cur[0], &_slab[0], buffer);
  }
  
  minc_1_writer::~minc_1_writer()
  {
    if(_set_image_range)
    {
      mivarput1(_mincid, _icmin, 0, NC_DOUBLE, NULL, &_image_range[0]);
      mivarput1(_mincid, _icmax, 0, NC_DOUBLE, NULL, &_image_range[1]);
      miset_valid_range(_mincid, _imgid, _image_range);
    }
  }
  
  void minc_1_writer::copy_headers(const minc_1_base& src)
  {
    
    //code copied from mincresample
    int nexcluded, excluded_vars[10];
    int varid;
    
    /* Create the list of excluded variables */
    nexcluded = 0;
    //ncopts = 0;

    if ((varid=ncvarid(src.mincid(), MIxspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MIyspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MIzspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MItime)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MIimage)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MIimagemax)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), MIimagemin)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
    if ((varid=ncvarid(src.mincid(), "rootvariable")) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
//#if MINC2
    if ((varid=ncvarid(src.mincid(), MIvector_dimension)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
//#endif /* MINC2 */
    //ncopts = NC_VERBOSE | NC_FATAL;
    /* Copy all other variable definitions */
    micopy_all_var_defs(src.mincid(), _mincid, nexcluded, excluded_vars);
  }
  
  //! append a line into minc history
  void minc_1_writer::append_history(const char *append_history)
  {
    nc_type datatype;
    int att_length;
    //ncopts=0;
    if ((ncattinq(_mincid, NC_GLOBAL, MIhistory, &datatype,&att_length) == MI_ERROR) ||
        (datatype != NC_CHAR))
      att_length = 0;
    att_length += strlen(append_history) + 1;
    char* str = new char[att_length];
    str[0] = '\0';
    miattgetstr(_mincid, NC_GLOBAL, (char*)MIhistory, att_length,str);
    //ncopts=NC_VERBOSE | NC_FATAL;
    strcat(str, append_history);
    miattputstr(_mincid, NC_GLOBAL, (char*)MIhistory, str);
    delete [] str;
  }
  
  int minc_1_base::create_var_id(const char *varname)
  {
    int old_ncopts = ncopts; ncopts = 0;
    int res=var_id(varname);
    if(res==MI_ERROR) //need to create a variable
      res=micreate_group_variable(_mincid,(char*)varname);//ncvardef(_mincid,varname,NC_INT,0,0);
    if(res==MI_ERROR) //need to create a variable
      res=ncvardef(_mincid,varname,NC_INT,0,0);
    ncopts = old_ncopts;
    return res;
  }
      
  void minc_1_base::insert(const char *varname,const char *attname,double val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_DOUBLE, 1, (void *) &val);
  }
  
  void minc_1_base::insert(const char *varname,const char *attname,const char* val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_CHAR, strlen(val) + 1, (void *) val);
  }
  
  void minc_1_base::insert(const char *varname,const char *attname,const std::vector<double> &val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_DOUBLE, val.size(), (void *) &val[0]);
  }
  
  void minc_1_base::insert(const char *varname,const char *attname,const std::vector<int> &val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_INT, val.size(), (void *) &val[0]);
  }
  
  void minc_1_base::insert(const char *varname,const char *attname,const std::vector<short> &val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_SHORT, val.size(), (void *) &val[0]);
  }
  
  void minc_1_base::insert(const char *varname,const char *attname,const std::vector<unsigned char> &val)
  {
    ncattput(_mincid, create_var_id(varname),attname, NC_BYTE, val.size(), (void *) &val[0]);
  }
  
};
