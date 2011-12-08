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

#ifndef __itkMincImageIO_h
#define __itkMincImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <itkImageIOBase.h>
#include <minc_1_simple.h>

namespace itk
{

 /** \class MincImageIO
  *
  * \author Vladimir S. Fonov
  * \brief Class that defines how to read Minc file format.
  * MINC IMAGE FILE FORMAT 
  *
  * \ingroup IOFilters
  */
  class ITK_EXPORT MincImageIO: public ImageIOBase
  {
  public:
    /** Standard class typedefs. */
    typedef MincImageIO  Self;
    typedef ImageIOBase  Superclass;
    typedef SmartPointer<Self>  Pointer;
  
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
  
    /** Run-time type information (and related methods). */
    itkTypeMacro(MincImageIO, Superclass);
  
    /*-------- This part of the interfaces deals with reading data. ----- */
  
    /** Determine if the file can be read with this ImageIO implementation.
         * \author Vladimir S. Fonov
         * \param FileNameToRead The name of the file to test for reading.
         * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
         * \return Returns true if this ImageIO can read the file specified.
         */
    virtual bool CanReadFile(const char* FileNameToRead) ;
  
    /** Set the spacing and dimension information for the set filename. */
    virtual void ReadImageInformation();
  
    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read(void* buffer);
  
    /*-------- This part of the interfaces deals with writing data. ----- */
  
    /** Determine if the file can be written with this ImageIO implementation.
         * \param FileNameToWrite The name of the file to test for writing.
         * \author Vladimir S. Fonov
         * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
         * \return Returns true if this ImageIO can write the file specified.
         */
    virtual bool CanWriteFile(const char * FileNameToWrite);
  
    /** Set the spacing and dimension information for the set filename. */
    virtual void WriteImageInformation();
  
    /** Writes the data to disk from the memory buffer provided.  */
    virtual void Write(const void* buffer);
  
  
  protected:
    //! minc 1 reader
    minc::minc_1_reader *_rdr; 
    minc::minc_1_writer *_wrt; 
    MincImageIO();
    ~MincImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const;
  
    MincImageIO(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
    void clean(void);
  };
} // end namespace itk

#endif // __itkMincImageIO_h
