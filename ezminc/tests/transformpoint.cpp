#include <volume_io.h>
#include <iostream>


int main(int argc, char **argv)
{
  if(argc<5)
  {
    std::cerr<<"Usage:"<<argv[0]<<"<XFM file> X Y Z"<<std::endl;
    return 1;
  }
  double x,y,z;
  double _x,_y,_z;
  
  x=atof(argv[2]);
  y=atof(argv[3]);
  z=atof(argv[4]);
  
  General_transform _xfm;
  if(input_transform_file((char*)argv[1], &_xfm)!=OK)
  {
    std::cerr<<"Error reading:"<<argv[1]<<std::endl;
    return 1;
  }
  general_transform_point( &_xfm, x, y, z,&_x, &_y, &_z);  
  delete_general_transform(&_xfm);
  std::cout<<_x<<" "<<_y<<" "<<_z<<std::endl;
  return 0;
};