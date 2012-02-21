/*=========================================================================
Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkMincImageIO.cxx,v $
Language:  C++
Date:      $Date: 2005/12/08 19:18:48 $
Version:   $Revision: 1.22 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkMincImageIO.h"
#include "itkIOCommon.h"
#include "itkExceptionObject.h"
#include "itkMetaDataObject.h"
#include <itkMatrix.h>
#include <itksys/SystemTools.hxx>
#include <vnl/vnl_math.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace minc;

namespace itk
{

  MincImageIO::MincImageIO():_rdr(NULL),_wrt(NULL)
  {
  }
  
  void MincImageIO::clean(void)
  {
    if(_rdr) delete _rdr;
    if(_wrt) delete _wrt;
    _rdr=NULL;
    _wrt=NULL;
  }
  
  MincImageIO::~MincImageIO()
  {
    clean();
  }
  
  void MincImageIO::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    if(_rdr)
      os<<"MINC file is opened for reading"<<std::endl;
    if(_wrt)
      os<<"MINC file is opened for writing"<<std::endl;
  }
  
  
  bool MincImageIO::CanReadFile( const char* FileNameToRead )
  {
    std::string filename = FileNameToRead;
    //taken from original MINC2ImageIO

    if( filename.empty())
    {
      itkDebugMacro(<<"No filename specified.");
      return false;
    }

    std::string::size_type mncPos = filename.rfind(".mnc");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".mnc2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }
    
    mncPos = filename.rfind(".MNC2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }
    
    mncPos = filename.rfind(".mnc.gz");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 7) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC.GZ");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 7) )
    {
      return true;
    }
    
    return false;
  }
  
  void MincImageIO::ReadImageInformation()
  {
    clean();
    try
    {
      _rdr=new minc_1_reader;
      _rdr->open(m_FileName.c_str(),true); //read in positive direction, always
      SetNumberOfDimensions((_rdr->ndim(1)>0?1:0)+(_rdr->ndim(2)>0?1:0)+(_rdr->ndim(3)>0?1:0));
      
      //SetMetaDataDictionary(thisDic);
      // set number of dimensions for ITK
      int image_max_length=_rdr->var_length(MIimagemax);
      int image_min_length=_rdr->var_length(MIimagemin);
      bool slice_normalized=image_max_length>1;
      /*
      std::cout<<"Image max length="<<image_max_length<<std::endl;
      std::cout<<"Image min length="<<image_min_length<<std::endl;*/
      
      switch(_rdr->datatype())
      {
        case NC_BYTE:
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=UCHAR;
          break;
          
        case NC_SHORT: 
          
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=_rdr->is_signed()?SHORT:USHORT;
          break;
          
        case NC_INT:
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=_rdr->is_signed()?INT:UINT;
          break;
          
        case NC_FLOAT:
          m_ComponentType=FLOAT;
          break;
        
        case NC_DOUBLE:
          m_ComponentType=DOUBLE;
          break;
        
        default:
          throw ExceptionObject(__FILE__,__LINE__,"Unsupported data type");
      }
      
      if(_rdr->ndim(0)==0 && _rdr->ndim(4)==0) //vector dimension
      {
        m_PixelType=SCALAR;
        //SetNumberOfComponents(1);
      } else {
        m_PixelType=VECTOR;
        if(_rdr->ndim(0)>0 && _rdr->ndim(4)>0)
          throw ExceptionObject(__FILE__,__LINE__,"Combining time and vector dimension in one file is not supported!");
        SetNumberOfComponents(_rdr->ndim(0)+_rdr->ndim(4));
      }
      
      for(int i=1,c=0; i < 4; i++)
      {
        if(_rdr->ndim(i)<=0) continue;
        SetDimensions(c,_rdr->ndim(i));
        SetSpacing(c,_rdr->nspacing(i));
        SetOrigin(c,_rdr->nstart(i));
        c++;
      }
      
      if(GetNumberOfDimensions()==3)
      {
        itk::Matrix< double, 3,3 > dir_cos;
        for(int i=1; i < 4; i++)
        {
          std::vector<double> dc(3);
          if(_rdr->have_dir_cos(i))
          {
            for(int j=0;j<3;j++)
            {
              dc[j]=_rdr->ndir_cos(i,j);
              dir_cos[j][i-1]=_rdr->ndir_cos(i,j);
            }
          } else {
            for(int j=0;j<3;j++)
            {
              dc[j]=((i-1)==j?1:0);
              dir_cos[j][i-1]=((i-1)==j?1:0);
            }
          }
          SetDirection(i-1,dc);
        }
        
        itk::Vector< double,3> origin;
        itk::Vector< double,3> o_origin;
        for(int j=0;j<3;j++)
          o_origin[j]=GetOrigin(j);
        origin=dir_cos*o_origin;
        for(int j=0;j<3;j++)
          SetOrigin(j,origin[j]);
      } else { //we are not rotating the origin according to direction cosines in this case 
        for(int i=1,c=0; i < 5; i++)
        {
          if(_rdr->ndim(i)<=0) continue;
          std::vector<double> dc(3);
          for(int j=0;j<3;j++)
            dc[j]=_rdr->ndir_cos(i,j);
          SetDirection(c,dc);
          c++;
        }
      }
      ComputeStrides();
      
      //now, we are just assuming that all the volumes with short,int and float data types are real, 
      //and those that are char are not 
      itk::MetaDataDictionary &thisDic=GetMetaDataDictionary();
      std::string classname(GetNameOfClass());
      itk::EncapsulateMetaData<std::string>(thisDic,ITK_InputFilterName, classname);
      //now let's store some metadata
      //internally image is always stored 
      
      /*itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
          (thisDic,ITK_CoordinateOrientation, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI);*/
      
      itk::EncapsulateMetaData(thisDic,"datatype",_rdr->datatype());
      itk::EncapsulateMetaData(thisDic,"signed"  ,_rdr->is_signed());
      
      /*switch(_rdr->datatype())
      {
        case NC_SHORT:
          
          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,_rdr->is_signed()?std::string(typeid(short).name()):std::string(typeid(unsigned short).name()));
          break;
          
        case NC_BYTE:
          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,_rdr->is_signed()?std::string(typeid(char).name()):std::string(typeid(unsigned char).name()));
          break;
          
        case NC_FLOAT:
          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,std::string(typeid(float).name()));
          break;
      }*/
      
      if(_rdr->ndim(4)) //we have got time dimension
      {
        itk::EncapsulateMetaData<double>(thisDic,"tstart",_rdr->nstart(4));
        itk::EncapsulateMetaData<double>(thisDic,"tstep",_rdr->nspacing(4));
      }
      
      std::vector<std::string> dimorder;
      for(int i=0;i<_rdr->dim_no();i++)
      {
        dimorder.push_back(_rdr->info()[i].name);
      }
      itk::EncapsulateMetaData(thisDic,"dimorder", dimorder);
      itk::EncapsulateMetaData<std::string>(thisDic,"history", _rdr->history());
      //walk through all the minc attributes, and store them
      for(int i=0;i<_rdr->var_number();i++)
      {
        std::string var=_rdr->var_name(i);
        
        if(var=="rootvariable" || 
           var=="image" || 
           var=="image-min" || 
           var=="image-max" ||
           var=="xspace" || 
           var=="yspace" ||
           var=="zspace" || 
           var=="time" ||
           var=="vector_dimension"  )
          continue;
        int var_id=_rdr->var_id(var.c_str());
        for(int j=0;j<_rdr->att_number(var_id);j++)
        {
          std::string att=_rdr->att_name(var_id,j);
          std::string path=var+":"+att;
          //std::cout<<path.c_str()<<" ";
          switch(_rdr->att_type(var_id,att.c_str()))
          {
            case NC_CHAR:
              itk::EncapsulateMetaData<std::string>(thisDic,path, _rdr->att_value_string(var_id,att.c_str()));
              //std::cout<<"string";
              break;
            case NC_INT:
              itk::EncapsulateMetaData<std::vector<int> >(thisDic,path, _rdr->att_value_int(var_id,att.c_str()));
              //std::cout<<"int";
              break;
            case NC_DOUBLE:
              itk::EncapsulateMetaData<std::vector<double> >(thisDic,path, _rdr->att_value_double(var_id,att.c_str()));
              //std::cout<<"double";
              break;
            case NC_SHORT:
              itk::EncapsulateMetaData<std::vector<short> >(thisDic,path, _rdr->att_value_short(var_id,att.c_str()));
              break;
            case NC_BYTE:
              itk::EncapsulateMetaData<std::vector<unsigned char> >(thisDic,path, _rdr->att_value_byte(var_id,att.c_str()));
              break;
            default:
              //std::cout<<"Unknown";
              break; //don't know what it is, skipping for now!  
          }
          //std::cout<<std::endl;
        }
      }
      
    } catch(const minc::generic_error & err) {
      throw ExceptionObject(__FILE__,__LINE__,"Error reading minc file");
    }
  }
    
  void MincImageIO::Read(void* buffer)
  {
    //TODO: add support for IORegion ?
    //ImageIORegion regionToRead = this->GetIORegion();
    switch(GetComponentType())
    {
      case CHAR:
      case UCHAR:
        _rdr->setup_read_byte(false);
        load_non_standard_volume<unsigned char>(*_rdr,(unsigned char*)buffer);
        break;
      case INT:
        _rdr->setup_read_int(false);
        load_non_standard_volume<int>(*_rdr,(int*)buffer);
        break;
      case UINT:
        _rdr->setup_read_uint(false);
        load_non_standard_volume<unsigned int>(*_rdr,(unsigned int*)buffer);
        break;
      case SHORT:
        _rdr->setup_read_short(false);
        load_non_standard_volume<short>(*_rdr,(short*)buffer);
        break;
      case USHORT:
        _rdr->setup_read_ushort(false);
        load_non_standard_volume<unsigned short>(*_rdr,(unsigned short*)buffer);
        break;
      case FLOAT:
        _rdr->setup_read_float();
        load_non_standard_volume<float>(*_rdr,(float*)buffer);
        break;
      case DOUBLE:
        _rdr->setup_read_double();
        load_non_standard_volume<double>(*_rdr,(double*)buffer);
        break;
      default:
        throw ExceptionObject(__FILE__, __LINE__,"Unsupported data type");
    }
    delete _rdr;
    _rdr=NULL;
  }
  
  bool MincImageIO::CanWriteFile(const char * FileNameToWrite)
  {
    std::string filename = FileNameToWrite;
    //taken from original MINC2ImageIO

    if( filename.empty())
    {
      itkDebugMacro(<<"No filename specified.");
      return false;
    }

    std::string::size_type mncPos = filename.rfind(".mnc");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".mnc2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }
    
    mncPos = filename.rfind(".MNC2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }
    
    return false;
  }
  
  void MincImageIO::WriteImageInformation(void) 
  {
    try
    {
      MetaDataDictionary &thisDic=GetMetaDataDictionary();
      nc_type datatype;
      bool is_signed=false;
      bool have_vectors=false;
      bool have_time=false;
      
      switch(GetComponentType())
      {
        case UCHAR:
        case CHAR:
          datatype=NC_BYTE;
          is_signed=false;
          break;
        case SHORT:
          datatype=NC_SHORT;
          is_signed=true;
          break;
        case USHORT:
          datatype=NC_SHORT;
          is_signed=false;
          break;
        case INT:
          datatype=NC_INT;
          is_signed=true;
          break;
        case UINT:
          datatype=NC_INT;
          is_signed=false;
       
          break;
        case FLOAT:
          //let's see if there is metadata available
          nc_type _datatype;
          bool _is_signed;
          if( itk::ExposeMetaData(thisDic,"datatype",_datatype) &&
              itk::ExposeMetaData(thisDic,"signed",_is_signed) ) 
          {
            datatype=_datatype;
            is_signed=_is_signed;
          } else {
            datatype=NC_FLOAT;
            is_signed=true;
          }
          
          break;
        case DOUBLE:
          datatype=NC_DOUBLE;
          is_signed=true;
          break;
        default:
        {
          throw ExceptionObject(__FILE__, __LINE__,"Unsupported data type");
        }
      }
      std::vector<std::string> dimorder;
      std::vector<int> dimmap(5,-1);
      minc_info info;
      
      if(GetNumberOfComponents()>1 && GetNumberOfComponents()<=3 ) 
      {
        have_vectors=true;
        have_time=false;
      } else if(GetNumberOfComponents()>3||GetNumberOfDimensions()>3) {
        have_vectors=false;
        have_time=true;
      }
      
      if(itk::ExposeMetaData(thisDic,"dimorder",dimorder))
      {
        //dimmap.resize(dimorder.size());
        for(int i=0,j=0;i<dimorder.size();i++)
        {
          if(dimorder[i]==MIvector_dimension && have_vectors)
          {
            dimmap[0]=j++;
          }
          else if(dimorder[i]==MItime && have_time)
          {
            dimmap[4]=j++;
          } else if(dimorder[i]==MIxspace) {
            dimmap[1]=j++;
          } else if(dimorder[i]==MIyspace) {
            dimmap[2]=j++;
          } else if(dimorder[i]==MIzspace) {
            dimmap[3]=j++;
          }
        }
      } else {
        if(have_vectors)
        {
          dimmap[0]=0;
        } else if(have_time) {
          have_vectors=false;
          have_time=true;
          dimmap[GetNumberOfDimensions()+1]=GetNumberOfDimensions();
        }
        dimmap[1]=(have_vectors?1:0)+0;
        dimmap[2]=(have_vectors?1:0)+1;
        dimmap[3]=(have_vectors?1:0)+2;
      }
      
      int dim_no=GetNumberOfDimensions()+(have_vectors||have_time?1:0);
      info.resize(dim_no);
      
/*      std::cout<<"dimmap:";
      for(int i=0;i<5;i++)
      {
        std::cout<<dimmap[i]<<",";
      }
      std::cout<<std::endl;*/
      
      for(int i=0;i<GetNumberOfDimensions();i++)
      {
        int _i=dimmap[i+1];//GetNumberOfDimensions()-i-1+(have_vectors?1:0);
        if(_i<0) 
        {
          throw ExceptionObject(__FILE__, __LINE__,"Internal error");
        }
         //ERROR!
        info[_i].length=GetDimensions(i);
        info[_i].step=GetSpacing(i);
        info[_i].start=GetOrigin(i);
        info[_i].have_dir_cos=true;
        for(int j=0;j<3;j++)
          info[_i].dir_cos[j]=GetDirection(i)[j];
        
        switch(i)
        {
          case 0:
            info[_i].dim=dim_info::DIM_X;
            break;
          case 1:
            info[_i].dim=dim_info::DIM_Y;
            break;
          case 2:
            info[_i].dim=dim_info::DIM_Z;
            break;
        }
      }
      if(GetNumberOfDimensions()==3) //we are only rotating 3D volumes
      {
        vnl_vector< double> start(3);
        vnl_vector< double> vorigin(3);
        itk::Matrix< double, 3,3 > _dir_cos;
        for(int i=0;i<3;i++)
        {
          vorigin[i]=GetOrigin(i);
          for(int j=0;j<3;j++)
            _dir_cos[j][i]=GetDirection(i)[j];
        }
        start=_dir_cos.GetInverse()*vorigin; //this is not optimal
        for(int i=0;i<3;i++)
        {
          int _i=dimmap[i+1];
          info[_i].start=start[i];
        }
      }
      //here we assume that we had a grid file
      if(have_vectors)
      {
        int _i=dimmap[0];
        info[_i].length=GetNumberOfComponents();
        info[_i].step=1;
        info[_i].start=0;
        info[_i].dim=dim_info::DIM_VEC;
        info[_i].have_dir_cos=false;
      } else if(have_time){
        int _i=dimmap[4];
        info[_i].length=GetNumberOfComponents();
        double tstep=1;
        double tstart=0;
        itk::ExposeMetaData(thisDic,"tstep",tstep);
        itk::ExposeMetaData(thisDic,"tstart",tstart);
        
        info[_i].step=tstep;
        info[_i].start=tstart;
        info[_i].dim=dim_info::DIM_TIME;
        info[_i].have_dir_cos=false;
      }
      
/*      std::cout<<"info:";
      for(int i=0;i<info.size();i++)
      {
        switch(info[i].dim)
        {
          case dim_info::DIM_VEC:
            std::cout<<"vector_dimension,";break;
          case dim_info::DIM_Z:
            std::cout<<"zspace,";break;
          case dim_info::DIM_Y:
            std::cout<<"yspace,";break;
          case dim_info::DIM_X:
            std::cout<<"xspace,";break;
          default: std::cout<<"Unknown! ";break;
            
        }
      }
      std::cout<<std::endl;      */
      
      //TODO:  shuffle info based on the dimnames
      _wrt=new minc_1_writer;
      _wrt->open(this->GetFileName(),
                 info,
                 (datatype==NC_FLOAT||datatype==NC_DOUBLE)?
                     dim_no:(have_time?dim_no-2:dim_no-1),
                 datatype,is_signed);
      
      //let's write some meta information if there is any 
      for(MetaDataDictionary::ConstIterator it=thisDic.Begin();it!=thisDic.End();++it)
      {
        const char *d=strchr((*it).first.c_str(),':');
        if(d)
        {
          std::string var((*it).first.c_str(),d-(*it).first.c_str());
          std::string att(d+1);
          itk::MetaDataObjectBase *bs=(*it).second;
          const char *tname=bs->GetMetaDataObjectTypeName();
          //std::cout<<var.c_str()<<"\t"<<att.c_str()<<"\t"<<tname<<std::endl;
          
          //THIS is not good OO style at all :(
          if(!strcmp(tname,typeid(std::string).name()))
          {
            _wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::string> *>(bs )->GetMetaDataObjectValue().c_str());
          } else if(!strcmp(tname,typeid(std::vector<double>).name())) {
            _wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::vector<double> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<int>).name())) {
            _wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::vector<int> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<short>).name())) {
            _wrt->insert(var.c_str(),att.c_str(),
                         dynamic_cast<MetaDataObject<std::vector<short> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<unsigned char>).name())) {
            _wrt->insert(var.c_str(),att.c_str(),
                         dynamic_cast<MetaDataObject<std::vector<unsigned char> > * >(bs)->GetMetaDataObjectValue());
          }
        } else if((*it).first=="history") {
          itk::MetaDataObjectBase *bs=(*it).second;
          _wrt->append_history(dynamic_cast<MetaDataObject<std::string> *>(bs)->GetMetaDataObjectValue().c_str());
        }
        
      }
    } catch(const minc::generic_error & err) {
      ExceptionObject exception(err.file(), err.line());
      std::string ErrorMessage="MINC IO error:";
      ErrorMessage+=err.msg();
      exception.SetDescription(ErrorMessage.c_str());
      throw exception;
    }
    return;
  }
  
  void MincImageIO::Write( const void* buffer)
  {
    try
    {
      this->WriteImageInformation(); //Write the image Information before writing data
      
      switch(this->GetComponentType())
      {
        case UCHAR:
        case CHAR:
          _wrt->setup_write_byte(false);
          save_non_standard_volume<unsigned char>(*_wrt,(const unsigned char*)buffer);
          break;
        case SHORT:
          _wrt->setup_write_short();
          save_non_standard_volume<short>(*_wrt,(const short*)buffer);
          break;
        case USHORT:
          _wrt->setup_write_ushort();
          save_non_standard_volume<unsigned short>(*_wrt,(const unsigned short*)buffer);
          break;
        case INT:
          _wrt->setup_write_int();
          save_non_standard_volume<int>(*_wrt,(const int*)buffer);
          break;
        case UINT:
          _wrt->setup_write_uint();
          save_non_standard_volume<unsigned int>(*_wrt,(const unsigned int*)buffer);
          break;
        case FLOAT:
          _wrt->setup_write_float();
          save_non_standard_volume<float>(*_wrt,(const float*)buffer);
          break;
        case DOUBLE:
          _wrt->setup_write_double();
          save_non_standard_volume<double>(*_wrt,(const double*)buffer);
          break;
        default:
          throw ExceptionObject(__FILE__,__LINE__,"Unsupported data type");
      }
      //finish writing, cleanup
      delete _wrt;
      _wrt=NULL;
    } catch(const minc::generic_error & err) {
      throw ExceptionObject(__FILE__, __LINE__,"Error reading minc file");
    }
    
  }
} // end namespace itk
