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
#ifndef __EXCPETIONS_H__
#define __EXCPETIONS_H__

#define REPORT_ERROR(MSG) throw minc::generic_error(__FILE__,__LINE__,MSG)

namespace minc
{
  class generic_error
  {
  public:
    const char *_file;
    int _line;
    const char *_msg;
    int _code;
  public:

    generic_error (const char *file, int line, const char *msg = "Error", int code = 0):
    _file (file), _line (line), _msg (msg), _code (code)
    {
      //                    std::cerr<<"Exception created: "<<_file<<":"<<_line<<" "<<_msg<<std::endl;
    }

    const char *file (void) const
    {
      return _file;
    }

    const char *msg (void) const
    {
      return _msg;
    }

    int line (void) const
    {
      return _line;
    }

    int code (void) const
    {
      return _code;
    }
  };
}; //minc
#endif //__EXCPETIONS_H__
