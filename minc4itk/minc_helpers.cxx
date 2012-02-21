/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@DESCRIPTION: 
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include <math.h>
#include "minc_helpers.h"

#include <itkMetaDataObject.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

namespace minc
{
	int get_image_limits (image3d::Pointer img, voxel_type &min,voxel_type &max)
	{
		//1. get min, max
		minc::image3d_iterator it (img, img->GetLargestPossibleRegion ());
		int count = 0;
	
		min = std::numeric_limits < voxel_type >::max ();
		  
		max = -std::numeric_limits < voxel_type >::max ();
		for (it.GoToBegin (); !it.IsAtEnd (); ++it)
		{
			voxel_type v=it.Value ();
			if (fabs(double(v)) > 1e20)
			  continue;
			count++;
			if (v < min)
			  min = v;
			if (v > max)
			  max = v;
		}
		return count;
	}
  
	int get_image_limits (mask3d::Pointer img, voxel_type &min,voxel_type &max)
	{
		//1. get min, max
		minc::mask3d_iterator it (img, img->GetLargestPossibleRegion ());
		int count = 0;
	
		min = std::numeric_limits < voxel_type >::max ();
		  
		max = -std::numeric_limits < voxel_type >::max ();
		for (it.GoToBegin (); !it.IsAtEnd (); ++it)
		{
			voxel_type v=it.Value ();
			if (fabs(double(v)) > 1e20)
			  continue;
			count++;
			if (v < min)
			  min = v;
			if (v > max)
			  max = v;
		}
		return count;
	}
	
	int get_image_limits (def3d::Pointer img, voxel_type &min, voxel_type &max)
	{
		//1. get min, max
		minc::def3d_iterator it (img, img->GetLargestPossibleRegion ());
		int count = 0;
	
		min = std::numeric_limits < voxel_type >::max ();
		max = -std::numeric_limits < voxel_type >::max ();
		
		for (it.GoToBegin (); !it.IsAtEnd (); ++it)
		{
			voxel_type va=v_max(it.Value () );
			voxel_type vi=v_min(it.Value () );
			if (fabs(double(va)) > 1e20)
			  continue;
			count++;
			if (vi < min)
			  min = vi;
			if (va > max)
			  max = va;
		}
		return count;
	}
  
  void normalize_mask(mask3d::Pointer img)
  {
		minc::mask3d_iterator it (img, img->GetLargestPossibleRegion ());
		for (it.GoToBegin (); !it.IsAtEnd (); ++it)
		{
			if(it.Value ()) it.Value ()=1;
		}
  }
	
	void write_tags(const tag_points& tags, const char * file)
	{
		std::ofstream out(file);
		if(!out.good()) REPORT_ERROR("Can't open file!");
		out<<"MNI Tag Point File"<<std::endl;
		out<<"Volumes = 1;"<<std::endl<<std::endl;
		out<<"Points =";
		//TODO add all the things
		for(tag_points::const_iterator i=tags.begin();i!=tags.end();i++)
		{
			out<<std::endl<<(*i)[0]<<" "<<(*i)[1]<< " "<<(*i)[2] <<" \"\"";
			if(!out.good()) REPORT_ERROR("Can't write to file!");
		}
		out << ";" << std::endl;
	}
  
	void write_tags(const tag_points& tags,const std::vector<int>& labels, const char * file)
	{
		std::ofstream out(file);
		if(!out.good()) REPORT_ERROR("Can't open file!");
		out<<"MNI Tag Point File"<<std::endl;
		out<<"Volumes = 1;"<<std::endl<<std::endl;
		out<<"Points =";
		//TODO add all the things
    std::vector<int>::const_iterator j;
    tag_points::const_iterator i;
		for(i=tags.begin(),j=labels.begin();i!=tags.end()&&j!=labels.end();i++,j++)
		{
			out<<std::endl<<(*i)[0]<<" "<<(*i)[1]<< " "<<(*i)[2] <<" \""<<*j<<"\"";
			if(!out.good()) REPORT_ERROR("Can't write to file!");
		}
		out << ";" << std::endl;
	}
  
  void write_tags(const tag_points& tags,const std::vector<float>& values, const char * file)
  {
		std::ofstream out(file);
		if(!out.good()) REPORT_ERROR("Can't open file!");
		out<<"MNI Tag Point File"<<std::endl;
		out<<"Volumes = 1;"<<std::endl<<std::endl;
		out<<"Points =";
		//TODO add all the things
    std::vector<float>::const_iterator j;
    tag_points::const_iterator i;
		for(i=tags.begin(),j=values.begin();i!=tags.end()&&j!=values.end();i++,j++)
		{
			out<<std::endl<<(*i)[0]<<" "<<(*i)[1]<< " "<<(*i)[2] <<" \""<<*j<<"\"";
			if(!out.good()) REPORT_ERROR("Can't write to file!");
		}
		out << ";" << std::endl;
    
  }
  
  void write_tags(const tag_points& tags,const std::vector<double>& values, const char * file)
  {
    std::ofstream out(file);
    if(!out.good()) REPORT_ERROR("Can't open file!");
    out<<"MNI Tag Point File"<<std::endl;
    out<<"Volumes = 1;"<<std::endl<<std::endl;
    out<<"Points =";
		//TODO add all the things
    std::vector<double>::const_iterator j;
    tag_points::const_iterator i;
    for(i=tags.begin(),j=values.begin();i!=tags.end()&&j!=values.end();i++,j++)
    {
      out<<std::endl<<(*i)[0]<<" "<<(*i)[1]<< " "<<(*i)[2] <<" \""<<*j<<"\"";
      if(!out.good()) REPORT_ERROR("Can't write to file!");
    }
    out << ";" << std::endl;
    
  }
	
  void write_2tags(const tag_points& tags,const tag_points& tags2, const char * file)
	{
		if(tags.size()!=tags2.size()) 
			REPORT_ERROR("Mismatching number of tags!");
		
		std::ofstream out(file);
		if(!out.good()) REPORT_ERROR("Can't open file!");
		out<<"MNI Tag Point File"<<std::endl;
		out<<"Volumes = 2;"<<std::endl<<std::endl;
		out<<"Points =";
		//TODO add all the things
		tag_points::const_iterator j=tags2.begin();
		tag_points::const_iterator i=tags.begin();
		for(;i!=tags.end();i++,j++)
		{
			out<<std::endl<<(*i)[0]<<" "<<(*i)[1]<< " "<<(*i)[2]<<" "<<(*j)[0]<<" "<<(*j)[1]<< " "<<(*j)[2] <<" \"\"";
			if(!out.good()) REPORT_ERROR("Can't write to file!");
		}
		out << ";" << std::endl;
	}
  
  static void split_string(char *str,std::vector<std::string>& lst,const char *delim=" ")
  {
    lst.clear();
    char *str2, *token;
    char *saveptr;
    while(str2=strtok_r(str,delim,&saveptr))
    {
      str=NULL;
      lst.push_back(str2);
    }
  }
	
	void read_tags(tag_points& tags, const char * file,int vol)
	{
		ifstream in(file);
		if(!in.good()) REPORT_ERROR("Can't open file!");
		//tags.clear();
		bool seen_points=false;
		const int _buffer_len=4095;
		char tmp[_buffer_len+1];
    int _vol=vol;
    
		while(!in.eof() )
		{
			in.getline(tmp,_buffer_len,'\n');
      if(tmp[0]=='%') continue;
			if(!in.good()) break;
			if(!seen_points) 
			{
        if(!strncmp(tmp,"Volumes =",9)) //got number of volumes 
        {
          char *del=strchr(tmp+9,';');
          if(del) *del=0;
          _vol=atoi(tmp+9);
          if(_vol<0 || _vol>2) _vol=1;

        }
        
				if(!strcmp(tmp,"Points =")) //found
				{
					seen_points=true;
					continue;
				}
			} else {
				std::istringstream str (std::string((const char *)tmp));
				tag_point tag;
				double dummy;
        
				if(_vol>1 && vol>1) //skip extra values
					str >> dummy >> dummy >> dummy;
        
				str>>tag[0]>>tag[1]>>tag[2];
				tags.push_back(tag);
			}
		}
	}
  
	void read_tags(tag_points& tags, std::vector<int>& labels, const char * file, int vol)
	{
		ifstream in(file);
		if(!in.good()) REPORT_ERROR("Can't open file!");
		//tags.clear();
		bool seen_points=false;
		const int _buffer_len=4095;
		char tmp[_buffer_len+1];
    int _vol=vol;
    
		while(!in.eof() )
		{
			in.getline(tmp,_buffer_len,'\n');
			if(!in.good()) break;
      if(tmp[0]=='%') continue;
      
			if(!seen_points) 
			{
        if(!strncmp(tmp,"Volumes =",9)) //got number of volumes 
        {
          char *del=strchr(tmp+9,';');
          if(del) *del=0;
          _vol=atoi(tmp+9);
          if(_vol<0 || _vol>2) _vol=1;
        }
        if(!strcmp(tmp,"Points =")) //found
				{
					seen_points=true;
					continue;
				}
			} else {
        std::vector<std::string> lst;
        tag_point tag;

        split_string(tmp,lst," ");
        int skip=0;
        
        if(lst.size()<3) continue;
        
        if(vol==2 && _vol==2 && lst.size()>5) //skip extra values
          skip=3;
        
        tag[0]=atof(lst[skip].c_str());
        tag[1]=atof(lst[skip+1].c_str());
        tag[2]=atof(lst[skip+2].c_str());
        
				tags.push_back(tag);
        
        if((lst.size()>6&&_vol==2) || lst.size()>3)
        {
          std::string lb=lst[lst.size()-1];
          
          //Analyze label
          std::string::size_type first=lb.find('"');
          std::string::size_type last=lb.rfind('"');
          if( first==std::string::npos || last==std::string::npos )
            labels.push_back(0);
          else
            labels.push_back(atoi(lb.substr(first+1,last-first).c_str()));
        } else 
          labels.push_back(0);
			}
		}
	}
  
  void read_tags(tag_points& tags, std::vector<double>& labels, const char * file, int vol)
  {
    ifstream in(file);
    if(!in.good()) REPORT_ERROR("Can't open file!");
		//tags.clear();
    bool seen_points=false;
    const int _buffer_len=4095;
    char tmp[_buffer_len+1];
    int _vol=vol;
    
    while(!in.eof() )
    {
      in.getline(tmp,_buffer_len,'\n');
      if(!in.good()) break;
      if(tmp[0]=='%') continue;
      
      if(!seen_points) 
      {
        if(!strncmp(tmp,"Volumes =",9)) //got number of volumes 
        {
          char *del=strchr(tmp+9,';');
          if(del) *del=0;
          _vol=atoi(tmp+9);
          if(_vol<0 || _vol>2) _vol=1;
        }
        if(!strcmp(tmp,"Points =")) //found
        {
          seen_points=true;
          continue;
        }
      } else {
        std::vector<std::string> lst;
        tag_point tag;

        split_string(tmp,lst," ");
        int skip=0;
        
        if(lst.size()<3) continue;
        
        if(vol==2 && _vol==2 && lst.size()>5) //skip extra values
          skip=3;
        
        tag[0]=atof(lst[skip].c_str());
        tag[1]=atof(lst[skip+1].c_str());
        tag[2]=atof(lst[skip+2].c_str());
        
        tags.push_back(tag);
        if((lst.size()>6&&_vol==2) || lst.size()>3)
        {
          std::string lb=lst[lst.size()-1];
          
          std::string::size_type first=lb.find('"');
          std::string::size_type last=lb.rfind('"');
          if( first==std::string::npos || last==std::string::npos )
            labels.push_back(0);
          else
            labels.push_back(atof(lb.substr(first+1,last-first).c_str()));
        } else 
           labels.push_back(0);
      }
    }
  }
  
  void read_tags(tag_points& tags, std::vector<float>& labels, const char * file, int vol)
  {
    ifstream in(file);
    if(!in.good()) REPORT_ERROR("Can't open file!");
		//tags.clear();
    bool seen_points=false;
    const int _buffer_len=4095;
    char tmp[_buffer_len+1];
    int _vol=vol;
    
    while(!in.eof() )
    {
      in.getline(tmp,_buffer_len,'\n');
      if(!in.good()) break;
      if(tmp[0]=='%') continue;
      
      if(!seen_points) 
      {
        if(!strncmp(tmp,"Volumes =",9)) //got number of volumes 
        {
          char *del=strchr(tmp+9,';');
          if(del) *del=0;
          _vol=atoi(tmp+9);
          if(_vol<0 || _vol>2) _vol=1;
        }
        if(!strcmp(tmp,"Points =")) //found
        {
          seen_points=true;
          continue;
        }
      } else {
        std::vector<std::string> lst;
        tag_point tag;

        split_string(tmp,lst," ");
        int skip=0;
        
        if(lst.size()<3) continue;
        
        if(vol==2 && _vol==2 && lst.size()>5) //skip extra values
          skip=3;
        
        tag[0]=atof(lst[skip].c_str());
        tag[1]=atof(lst[skip+1].c_str());
        tag[2]=atof(lst[skip+2].c_str());
        
        tags.push_back(tag);
        if((lst.size()>6&&_vol==2) || lst.size()>3)
        {
          std::string lb=lst[lst.size()-1];
          
          std::string::size_type first=lb.find('"');
          std::string::size_type last=lb.rfind('"');
          if( first==std::string::npos || last==std::string::npos )
            labels.push_back(0);
          else
            labels.push_back(atof(lb.substr(first+1,last-first).c_str()));
        } else 
          labels.push_back(0);
      }
    }
  }
  
  void read_tags(tag_points& tag1, tag_points& tag2,std::vector<double>& labels, const char * file)
  {
    ifstream in(file);
    if(!in.good()) REPORT_ERROR("Can't open file!");
    //tags.clear();
    bool seen_points=false;
    const int _buffer_len=4095;
    char tmp[_buffer_len+1];
    int _vol=2;
    
    while(!in.eof() )
    {
      in.getline(tmp,_buffer_len,'\n');
      if(!in.good()) break;
      if(tmp[0]=='%') continue;
      
      if(!seen_points) 
      {
        if(!strncmp(tmp,"Volumes =",9)) //got number of volumes 
        {
          char *del=strchr(tmp+9,';');
          if(del) *del=0;
          _vol=atoi(tmp+9);
          if(_vol<0 || _vol>2) _vol=1;
        }
        if(!strcmp(tmp,"Points =")) //found
        {
          seen_points=true;
          continue;
        }
      } else {
        std::vector<std::string> lst;
        tag_point tag;

        split_string(tmp,lst," ");
        int skip=0;
        
        if(lst.size()<3) continue;
        
        tag[0]=atof(lst[0].c_str());
        tag[1]=atof(lst[1].c_str());
        tag[2]=atof(lst[2].c_str());
        tag1.push_back(tag);
        
        if(lst.size()>5 && _vol==2)
        {
          tag[0]=atof(lst[0].c_str());
          tag[1]=atof(lst[1].c_str());
          tag[2]=atof(lst[2].c_str());
        } else {
          tag[0]=0;
          tag[1]=0;
          tag[2]=0;
        }
        tag2.push_back(tag);
        
        if((lst.size()>6&&_vol==2) || lst.size()>3)
        {
          std::string lb=lst[lst.size()-1];
          std::string::size_type first=lb.find('"');
          std::string::size_type last=lb.rfind('"');
          if( first==std::string::npos || last==std::string::npos )
            labels.push_back(0);
          else
            labels.push_back(atof(lb.substr(first+1,last-first).c_str()));
        } else 
          labels.push_back(0);
      }
    }
  }
  

  void load_parameters(const char *file,itk::Array<double> &param)
  {
    std::vector<double> p;
    ifstream in(file);
    if(!in.good()) 
      REPORT_ERROR("Can't open the file for reading!");
    copy(istream_iterator<double>(in), istream_iterator<double>(), back_inserter(p));
    if(p.empty()) // probably this is not right!
      REPORT_ERROR("Can't read parameters!");
    param.SetSize(p.size());
    for(int i=0;i<p.size();i++)
      param[i]=p[i];
  }
  
  void load_parameters(const char *file,itk::Array<double> &param,int n)
  {
    std::vector<double> p;
    ifstream in(file);
    if(!in.good()) 
      REPORT_ERROR("Can't open the file for reading!");
    copy(istream_iterator<double>(in), istream_iterator<double>(), back_inserter(p));
    if(p.empty()) // probably this is not right!
      REPORT_ERROR("Can't read parameters!");
    param.SetSize(n);
    param.Fill(0);
    
    for(int i=0;i<p.size() && i<n;i++)
      param[i]=p[i];
  }

  void save_parameters(const char *file,const itk::Array<double> &param)
  {
    std::vector<double> p;
    ofstream out(file);
    if(!out.good())
      REPORT_ERROR("Can't open the file for writing!");
    out.precision(40);
    for(int i=0;i<param.Size();i++)
      out<<param[i]<<endl;
  }
  
  //simplistic XFM file parsing
  void read_linear_xfm(const char *xfm,itk::Matrix<double,3,3>& rot, itk::Vector<double,3>& tran )
  {
    rot.SetIdentity();
    tran.Fill(0);
    std::ifstream in(xfm);
    char tmp[1024];
    bool found_transform=false;
    while(!in.eof() && in.good())
    {
      in.getline(tmp,1023);
      if(strstr(tmp,"Linear_Transform =")) // ok, found the description
      { 
          found_transform=true;
          break;
      }
    }
    if(found_transform)
    {
      for(int c=0;c<3;c++)
      {
        in>>rot[c][0]
          >>rot[c][1]
          >>rot[c][2]
          >>tran[c];
      }
    }
  }
  
  void read_linear_xfm(const char *xfm,itk::Matrix<double,2,2>& rot, itk::Vector<double,2>& tran )
  {
    itk::Matrix<double,3,3> r;
    itk::Vector<double,3> t;
    read_linear_xfm(xfm,r,t);
    
    for(int i=0;i<2;i++)
    {
      for(int j=0;j<2;j++)
      {
        rot[i][j]=r[i][j];
      }
      tran[i]=t[i];
    }
  }


  void write_linear_xfm(const char *xfm,const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran )
  {
      std::ofstream out(xfm);
      out.precision(10);
      out<<"MNI Transform File"<<std::endl;
      out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
      for(int c=0;c<3;c++)
      {
        out<<std::endl<<rot[c][0]<<" "
                      <<rot[c][1]<<" "
                      <<rot[c][2]<<" "
                      <<tran[c];
      }
      out<<";"<<std::endl;
  }
  
  void write_linear_xfm(const char *xfm,const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran )
  {
    std::ofstream out(xfm);
    out.precision(10);
    out<<"MNI Transform File"<<std::endl;
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<2;c++)
    {
      out<<std::endl<<rot[c][0]<<" "
          <<rot[c][1]<<" "
          <<0<<" "
          <<tran[c];
    }

    out<<std::endl<<0<<" "
        <<0<<" "
        <<0<<" "
        <<0;

    out<<";"<<std::endl;
  }

  
  void write_combined_xfm(const char *xfm,const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran, const char *grid )
  {
    std::ofstream out(xfm);
    out.precision(10);
    out<<"MNI Transform File"<<std::endl;
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<3;c++)
    {
      out<<std::endl<<rot[c][0]<<" "
          <<rot[c][1]<<" "
          <<rot[c][2]<<" "
          <<tran[c];
    }
    out<<";"<<std::endl;
    
    out<<"Transform_Type = Grid_Transform;"<<std::endl<<"Displacement_Volume = "<<grid<<";"<<std::endl;
  }
  
  void write_combined_xfm(const char *xfm,const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran, const char *grid )
  {
    std::ofstream out(xfm);
    out.precision(10);
    out<<"MNI Transform File"<<std::endl;
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<2;c++)
    {
      out<<std::endl<<rot[c][0]<<" "
          <<rot[c][1]<<" "
          <<0<<" "
          <<tran[c];
    }

    out<<std::endl<<0<<" "
        <<0<<" "
        <<0<<" "
        <<0;

    out<<";"<<std::endl;
    
    out<<"Transform_Type = Grid_Transform;"<<std::endl<<"Displacement_Volume = "<<grid<<";"<<std::endl;
    
  }
  
  void write_combined_xfm(const char *xfm, const char *grid,const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran)
  {
    std::ofstream out(xfm);
    out.precision(10);
    out<<"MNI Transform File"<<std::endl;
    
    out<<"Transform_Type = Grid_Transform;"<<std::endl<<"Displacement_Volume = "<<grid<<";"<<std::endl;
    
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<3;c++)
    {
      out<<std::endl<<rot[c][0]<<" "
          <<rot[c][1]<<" "
          <<rot[c][2]<<" "
          <<tran[c];
    }
    out<<";"<<std::endl;
  }
  
  void write_combined_xfm(const char *xfm, const char *grid,const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran )
  {
    std::ofstream out(xfm);
    out.precision(10);
    out<<"MNI Transform File"<<std::endl;
    
    out<<"Transform_Type = Grid_Transform;"<<std::endl<<"Displacement_Volume = "<<grid<<";"<<std::endl;
    
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<2;c++)
    {
      out<<std::endl<<rot[c][0]<<" "
          <<rot[c][1]<<" "
          <<0<<" "
          <<tran[c];
    }

    out<<std::endl<<0<<" "
        <<0<<" "
        <<0<<" "
        <<0;

    out<<";"<<std::endl;
  }
  
  void write_nonlinear_xfm(const char *xfm,const char *grid)
  {
    std::ofstream out(xfm);
    out<<"MNI Transform File"<<std::endl;
    out<<"Transform_Type = Linear;"<<std::endl<<"Linear_Transform =";
    for(int c=0;c<3;c++)
    {
      out<<std::endl<<(c==0?1:0)<<" " <<(c==1?1:0)<<" " <<(c==2?1:0)<<" " <<0;
    }
    out<<";"<<std::endl;
    
    out<<"Transform_Type = Grid_Transform;"<<std::endl<<" Displacement_Volume = "<<grid<<";"<<std::endl;
  }
  
  void set_minc_storage_type(itk::Object* image,nc_type datatype,bool is_signed)
  {
    itk::EncapsulateMetaData( image->GetMetaDataDictionary(),"datatype",datatype);
    itk::EncapsulateMetaData( image->GetMetaDataDictionary(),"signed",is_signed);
  }
  
  void copy_metadata(itk::Object* dst,itk::Object* src)
  {
    dst->SetMetaDataDictionary(src->GetMetaDataDictionary());
  }
  
  void copy_dimorder(itk::Object* dst,itk::Object* src)
  {
    std::vector<std::string> dimorder;
    if(itk::ExposeMetaData(src->GetMetaDataDictionary(),"dimorder",dimorder))
    {
      itk::EncapsulateMetaData(dst->GetMetaDataDictionary(),"dimorder",dimorder);
    }
  }
  
  void append_history(itk::Object* dst,const std::string& history)
  {
    std::string old_history;
    itk::ExposeMetaData<std::string>( dst->GetMetaDataDictionary() , "history",old_history);
    old_history+=history;
    itk::EncapsulateMetaData( dst->GetMetaDataDictionary(),"history",old_history);
  }

  
};//minc

