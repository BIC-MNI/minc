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
#ifndef __FIXED_VECTOR_H__
#define __FIXED_VECTOR_H__

#include <limits>

namespace minc
{
  //! fixed size array, which support arithmetic operations
  template<int dim,class I=int> class fixed_vec
  {
  protected:
    I c[dim];
  public:
    //! default constructor, does nothing (i.e data is uninitilized)
    fixed_vec() {}
  
    //! constructor which sets all the elements to the same value
    explicit fixed_vec(I v)
    {
        for(unsigned int i=0;i<dim;i++)
            c[i]=v;
    }

    //! constructor which sets all the elements to be a copy of C-array
    explicit fixed_vec(const I* v)
    {
        for(unsigned int i=0;i<dim;i++)
            c[i]=v[i];
    }
    
    //! conversion to the C array
    I* c_buf()
    {
      return c;
    }
	
    //! conversion to const C array 
    const I* c_buf() const
    {
      return c;
    }

    //! element access operator
    I& operator[](int i)
    {
#ifdef _INDEX_CHECK
      if(i<0 || i>=dim) REPORT_ERROR("Index out of bounds");
#endif //_INDEX_CHECK
      return c[i];
    }
    //! const element access operator
    I operator[](int i) const
    {
#ifdef _INDEX_CHECK
      if(i<0 || i>=dim) REPORT_ERROR("Index out of bounds");
#endif //_INDEX_CHECK
      return c[i];
    }

    //! const element access operator
    I get(int i)
    {
      return (*this)[i];
    }
    //! element writing operator
    void set(int i,I v)
    {
      (*this)[i]=v;
    }

    //! find a maximum of elements
    I max(void) const
    {
      I s=std::numeric_limits < I >::min ();;
      for(unsigned int i=0;i<dim;i++)
        if(c[i]>s) s=c[i];
      return s;
    }
	
    //! find a minimum of elements
    I min(void) const
    {
      I s=std::numeric_limits < I >::max ();;
      for(unsigned int i=0;i<dim;i++)
        if(c[i]<s) s=c[i];
      return s;
    }
	
    //! calculate sum of all elements
    I sum(void) const
    {
      I s=0;
      for(unsigned int i=0;i<dim;i++)
        s+=c[i];
      return s;
    }

    //! modulus squared
    I mod2(void) const
    {
      I s=0;
      for(unsigned int i=0;i<dim;i++)
        s+=c[i]*c[i];
      return s;
    }
    
    //! volume (product of all elements)
    I vol(void) const
    {
      I s=1;
      for(unsigned int i=0;i<dim;i++)
        s*=c[i];
      return s;
    }
	
    //! \name fixed_vec arithmentic operations
    //@{
    fixed_vec<dim,I>& operator *=(const fixed_vec<dim,I>& b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]*=b[i];
      return *this;
    }

    fixed_vec<dim,I>& operator *=(const I b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]*=b;
      return *this;
    }

    fixed_vec<dim,I>& operator +=(const fixed_vec<dim,I>& b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]+=b[i];
      return *this;
    }

    fixed_vec<dim,I>& operator -=(const fixed_vec<dim,I>& b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]-=b[i];
      return *this;
    }

    fixed_vec<dim,I>& operator /=(const fixed_vec<dim,I>& b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]/=b[i];
      return *this;
    }

    fixed_vec<dim,I>& operator /=(const I b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]/=b;
      return *this;
    }

    fixed_vec<dim,I> operator /(const I b)
    {
      fixed_vec<dim,I> tmp;
      for(unsigned int i=0;i<dim;i++)
        tmp[i]=c[i]/b;
      return tmp;
    }

    fixed_vec<dim,I> operator *(const I b)
    {
      fixed_vec<dim,I> tmp;
      for(unsigned int i=0;i<dim;i++)
        tmp[i]=c[i]*b;
      return tmp;
    }

    fixed_vec<dim,I> operator -(const fixed_vec<dim,I>& b)
    {
      fixed_vec<dim,I> tmp;
      for(unsigned int i=0;i<dim;i++)
        tmp[i]=c[i]-b[i];
      return tmp;
    }

    fixed_vec<dim,I> operator +(const fixed_vec<dim,I>& b)
    {
      fixed_vec<dim,I> tmp;
      for(unsigned int i=0;i<dim;i++)
        tmp[i]=c[i]+b[i];
      return tmp;
    }

    //@}

    //! assignement operator, copies contents
    fixed_vec<dim,I>& operator=(const fixed_vec<dim,I>& b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]=b[i];
      return *this;
    }

    //! assignement operator, copies contents, assumes that b have at least this size
    fixed_vec<dim,I>& operator=(const I* b)
    {
      for(unsigned int i=0;i<dim;i++)
        c[i]=b[i];
      return *this;
    }

    //! assignement operator, sets all elements to the same value
    fixed_vec<dim,I>& operator=(const I b)
    {
      for(unsigned int i=0;i<dim;i++) c[i]=b;
      return *this;
    }

    //! inequality operator, does element wise equality check
    bool operator!=(const fixed_vec<dim,I>& b) const
    {
      for(int i=0;i<dim;i++) if(c[i]!=b[i]) return true;
      return false;
    }
	
    //! equality operator, does element wise equality check
    bool operator==(const fixed_vec<dim,I>& b) const
    {
      for(int i=0;i<dim;i++) if(c[i]!=b[i]) return false;
      return true;
    }
    
    //! reverse the order of elements
    void reverse(void)
    {
      for(int i=0;i<dim/2;i++)
      {
        I tmp=c[i];
        c[i]=c[dim-i-1];
        c[dim-i-1]=tmp;
      }
    }
  };

  //! element wise division
	template<int dim,class I> fixed_vec<dim,I> operator/(const fixed_vec<dim,I> &l,const fixed_vec<dim,I> &r)
	{
		fixed_vec<dim,I> out=l;
		out/=r;
		return out; //this is not effecient  - no return value optimisation
	}
  
	//! element wise multiplication
	template<int dim,class I> fixed_vec<dim,I> operator*(const fixed_vec<dim,I> &l,const fixed_vec<dim,I> &r)
	{
		fixed_vec<dim,I> out=l;
		out*=r;
		return out; //this is not effecient  - no return value optimisation
	}
  
	//! element wise addition
	template<int dim,class I> fixed_vec<dim,I> operator+(const fixed_vec<dim,I> &l,const fixed_vec<dim,I> &r)
	{
		fixed_vec<dim,I> out=l;
		out+=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
  //! element wise subtraction
	template<int dim,class I> fixed_vec<dim,I> operator-(const fixed_vec<dim,I> &l,const fixed_vec<dim,I> &r)
	{
		fixed_vec<dim,I> out=l;
		out-=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
	//! devide all elements by a value
	template<int dim,class I> fixed_vec<dim,I> operator/(const fixed_vec<dim,I> &l,I r)
	{
		fixed_vec<dim,I> out=l;
		out/=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
  //! multiply all elements by a value 
	template<int dim,class I> fixed_vec<dim,I> operator*(const fixed_vec<dim,I> &l,I r)
	{
		fixed_vec<dim,I> out=l;
		out*=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
  //! add a value to all elements 
	template<int dim,class I> fixed_vec<dim,I> operator+(const fixed_vec<dim,I> &l,I r)
	{
		fixed_vec<dim,I> out=l;
		out+=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
  //! subtract a value from all elements 
	template<int dim,class I> fixed_vec<dim,I> operator-(const fixed_vec<dim,I> &l,I r)
	{
		fixed_vec<dim,I> out=l;
		out-=r;
		return out; //this is not effecient  - no return value optimisation
	}
	
	//! create 1d fixed_vec
	template<class I> fixed_vec<1,I> IDX(I i)
	{
		fixed_vec<1,I> d;
		d[0]=i;
		return d;
	}
	
	//! create 2d fixed_vec
	template<class I> fixed_vec<2,I> IDX(I i,I j)
	{
		fixed_vec<2,I> d;
		d[0]=i;
		d[1]=j;
		return d;
	}
	
	//! create 3d fixed_vec
	template<class I> fixed_vec<3,I> IDX(I i,I j,I k)
	{
		fixed_vec<3,I> d;
		d[0]=i;
		d[1]=j;
		d[2]=k;
		return d;
	}
	
	//! create 4d fixed_vec
	template<class I> fixed_vec<4,I> IDX(I i,I j,I k,I l)
	{
		fixed_vec<3,I> d;
		d[0]=i;
		d[1]=j;
		d[2]=k;
		d[3]=l;
		return d;
	}
	
	//!average value of a vector
	template<class T,int d>T AVG(const fixed_vec<d,T> &v)
	{
		return v.sum()/d;
	}	
  
	//!dot product of two vectors
  template<class T,int d>T dot(const fixed_vec<d,T> &v1,const fixed_vec<d,T> &v2)
  {
    T val=0;
    for(int i=0;i<d;i++) val+=v1[i]*v2[i];
    return val;
  }	
	
};

#endif //__FIXED_VECTOR_H__
