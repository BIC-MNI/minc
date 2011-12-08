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
#ifndef __MINC_GENERAL_TRANSFORM_H__
#define __MINC_GENERAL_TRANSFORM_H__

#include <itkObject.h>
#include <itkPoint.h>
#include <itkVector.h>
#include <itkCovariantVector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_vector_fixed_ref.h>
#include <vnl/vnl_vector.h>

#include <itkTransform.h>
#include <itkObjectFactory.h>

//minc header
#include <volume_io.h>

namespace minc
{
  /** \class XfmTransform
   * \brief ITK wrapper around Minc general transform functions.
   *
   * \ingroup Transforms
   *
   */
  template < class TScalarType=double, unsigned int NInputDimensions=3,unsigned int NOutputDimensions=3> 
   class XfmTransform : public itk::Transform < TScalarType, NInputDimensions, NOutputDimensions>
  {
  public:
    /** Standard class typedefs. */
    typedef XfmTransform  Self;
    typedef typename itk::Transform < TScalarType, NInputDimensions, NOutputDimensions> Superclass;
    typedef typename itk::SmartPointer< Self >   Pointer;
    typedef typename itk::SmartPointer< const Self >  ConstPointer;
    
    
    /** New method for creating an object using a factory. */
    itkNewMacro(Self);
  
    /** Run-time type information (and related methods). */
    itkTypeMacro( XfmTransform, itk::Transform );
  
    /** Dimension of the domain space. */
    itkStaticConstMacro(InputSpaceDimension, unsigned int, NInputDimensions);
    itkStaticConstMacro(OutputSpaceDimension, unsigned int, NOutputDimensions);
    
    /** Type of the input parameters. */
    
    typedef  double ScalarType;
  
    /** Type of the input parameters. */
    typedef typename Superclass::ParametersType ParametersType;
  
    /** Type of the Jacobian matrix. */
    typedef typename Superclass::JacobianType  JacobianType;
  
    /** Standard vector type for this class. */
    typedef itk::Vector<TScalarType,
                  itkGetStaticConstMacro(InputSpaceDimension)>  InputVectorType;
    typedef itk::Vector<TScalarType,
                  itkGetStaticConstMacro(OutputSpaceDimension)> OutputVectorType;
    
    /** Standard covariant vector type for this class */
    typedef itk::CovariantVector<TScalarType,
                            itkGetStaticConstMacro(InputSpaceDimension)>  InputCovariantVectorType;
                            
    typedef itk::CovariantVector<TScalarType,
                            itkGetStaticConstMacro(OutputSpaceDimension)> OutputCovariantVectorType;
    
    /** Standard coordinate point type for this class */
    typedef itk::Point<TScalarType,NInputDimensions > InputPointType;
  
    typedef itk::Point<TScalarType,NInputDimensions > OutputPointType;
    
    /**  Method to transform a point. */
    virtual OutputPointType TransformPoint(const InputPointType  &point ) const
    {
      if(!_initialized) return point;
      if(_invert && !_initialized_invert)  return point;
      OutputPointType pnt;
      if(NInputDimensions==3 && NOutputDimensions==3)
	general_transform_point((_invert ? &_xfm_inv : &_xfm), point[0], point[1], point[2],&pnt[0], &pnt[1], &pnt[2]);
      else
	itkExceptionMacro(<< "Sorry, only 3D to 3d minc xfm transform is currently implemented");
      return pnt;
    }
    //! use finate element difference to estimate local jacobian 
    void estimate_local_jacobian(const InputPointType  &orig, vnl_matrix_fixed< double, 3, 3 > &m)
    {
      double u1,v1,w1;
      double u2,v2,w2;
      const double delta=1e-4;

      general_transform_point((_invert ? &_xfm_inv : &_xfm) , orig[0]-delta, orig[1], orig[2],&u1, &v1, &w1);
      general_transform_point((_invert ? &_xfm_inv : &_xfm) , orig[0]+delta, orig[1], orig[2],&u2, &v2, &w2);
      m(0,0)=(u2-u1)/(2*delta);
      m(0,1)=(v2-v1)/(2*delta);
      m(0,2)=(w2-w1)/(2*delta);
      
      general_transform_point((_invert ? &_xfm_inv : &_xfm) , orig[0], orig[1]-delta, orig[2],&u1, &v1, &w1);
      general_transform_point((_invert ? &_xfm_inv : &_xfm) , orig[0], orig[1]+delta, orig[2],&u2, &v2, &w2);
      m(1,0)=(u2-u1)/(2*delta);
      m(1,1)=(v2-v1)/(2*delta);
      m(1,2)=(w2-w1)/(2*delta);
      
      general_transform_point((_invert ? &_xfm_inv : &_xfm), orig[0], orig[1], orig[2]-delta,&u1, &v1, &w1);
      general_transform_point((_invert ? &_xfm_inv : &_xfm), orig[0], orig[1], orig[2]+delta,&u2, &v2, &w2);
      m(2,0)=(u2-u1)/(2*delta);
      m(2,1)=(v2-v1)/(2*delta);
      m(2,2)=(w2-w1)/(2*delta);
    }
    
    /**  Method to transform a vector. */
    virtual OutputVectorType TransformVector(const InputVectorType &vector) const 
    {
      itkExceptionMacro( << "Not Implemented" );
      return vector; 
    }
  
    /**  Method to transform a vnl_vector. */
/*    virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &vector) const
    {
      itkExceptionMacro( << "Not Implemented" );
      return vector; 
    }*/
  
    
    /**  Method to transform a CovariantVector. */
    virtual OutputCovariantVectorType TransformCovariantVector(
      const InputCovariantVectorType &vector) const
    {
      itkExceptionMacro( << "Not Implemented" );
      return vector; 
    }

    
    /** Set the transformation to an Identity
     */
    virtual void SetIdentity( void )
    {
      _cleanup();
    }
    
#if ( ITK_VERSION_MAJOR > 3 ) 
    virtual void SetFixedParameters(const ParametersType &)
    {
      itkExceptionMacro( << "Not Implemented" );
    }
    
    virtual void ComputeJacobianWithRespectToParameters(
                const InputPointType &,
                JacobianType &) const
    {
      itkExceptionMacro( << "Not Implemented" );
    }
    
   virtual void ComputeJacobianWithRespectToPosition(
                const InputPointType & x,
                JacobianType &jacobian ) const
    {
      itkExceptionMacro( << "Not Implemented" );
    }
    
#endif

    
#if ( ITK_VERSION_MAJOR > 3 ) 
    virtual NumberOfParametersType GetNumberOfParameters(void) const
#else
    virtual unsigned int GetNumberOfParameters(void) const
#endif
    {
      //this transform is defined by XFM file
      itkExceptionMacro( << "Not Defined" );
      return 0;
    }
    
    /** Set the Transformation Parameters
     * and update the internal transformation. */
    virtual void  SetParameters(const ParametersType & param)
    {
      itkExceptionMacro( << "Not Implemented" );
    }
    
    virtual const ParametersType & GetParameters(void)
    {
      itkExceptionMacro( << "Not Implemented" );
      return _parameters;
    }
    
#if ( ITK_VERSION_MAJOR == 3 ) 
    virtual const JacobianType & GetJacobian(const InputPointType  & point) const
    { 
      itkExceptionMacro( << "Not Implemented" );
      return this->m_Jacobian;
    }
#endif    
    
    void OpenXfm(const char *xfm)
    {
      _cleanup();
      if(input_transform_file((char*)xfm, &_xfm)!=OK)
        itkExceptionMacro( << "Error reading XFM:" << xfm );
      _initialized=true;
    }
    
    void Invert(void)
    {
      if(!_initialized)
        itkExceptionMacro( << "XFM not initialized" );
      if(!_initialized_invert)
      {
        create_inverse_general_transform(&_xfm,&_xfm_inv);
        _initialized_invert=true;
      }
      _invert= !_invert;
    }
    
  protected:
    
#if ( ITK_VERSION_MAJOR > 3 ) 
    XfmTransform(): itk::Transform< TScalarType, NInputDimensions, NOutputDimensions>(0),_invert(false),_initialized(false),_initialized_invert(false)
    { 
    }
#else  
    XfmTransform(): itk::Transform< TScalarType, NInputDimensions, NOutputDimensions>(NInputDimensions,NOutputDimensions),_invert(false),_initialized(false),_initialized_invert(false)
    { 
    }
#endif  
    
    virtual ~XfmTransform() 
    {
      _cleanup();
    }
      
    void _cleanup(void)
    {
      if(_initialized)
      {
        delete_general_transform(&_xfm);
      }
      if(_initialized_invert)
      {
        delete_general_transform(&_xfm_inv);
      }
      _initialized=false;
      _initialized_invert=false;
    }
    
    ParametersType _parameters;
    mutable General_transform _xfm,_xfm_inv;//quick fix for general_transform
    bool _invert;
    bool _initialized,_initialized_invert;
    
  private:
    XfmTransform ( const Self & ); //purposely not implemented
    void operator= ( const Self & ); //purposely not implemented
  };

  
};
#endif //__MINC_GENERAL_TRANSFORM_H__
