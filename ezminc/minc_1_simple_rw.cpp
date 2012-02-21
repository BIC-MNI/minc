#include <iostream>
#include <math.h>

#include "minc_1_simple_rw.h"

namespace minc
{

  const double minc_eps=1e-5;
  
  bool is_same(minc_1_reader& one,minc_1_reader& two,bool verbose)
  {
    if(one.dim_no()!=two.dim_no())
    {
      if(verbose)
        std::cerr<<"Unequal number of dimensions !"<<std::endl;
      return false;
    }
    for(int j=0;j<5;j++)
    {
      if(one.ndim(j)!=two.ndim(j))
      {
        if(verbose)
          std::cerr<<"Unequal dimension sizes"<<std::endl;
        return false;
      }
      
      if(fabs(one.nstart(j)-two.nstart(j))>minc_eps)
      {
        if(verbose)
          std::cerr<<"Unequal dimension sarts"<<std::endl;
        return false;
      }
      
      if(fabs(one.nspacing(j)-two.nspacing(j))>minc_eps)
      {
        if(verbose)
          std::cerr<<"Unequal dimension steps"<<std::endl;
        return false;
      }
      
      for(int i=0;i<3;i++)
        if(fabs(one.ndir_cos(j,i)-two.ndir_cos(j,i))>minc_eps)
        {
          if(verbose)
            std::cerr<<"Unequal direction cosines"<<std::endl;
          return false;
        }
    }
    return true;
  }

};
