/* ----------------------------- MNI Header -----------------------------------
@NAME       :  trilinear_resample
@DESCRIPTION:  an example of using trilinear resampling algorithm
@COPYRIGHT  :
              Copyright 2011 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include "minc_1_rw.h"
#include <iostream>
#include "minc_1_simple.h"
#include "minc_1_simple_rw.h"
#include <getopt.h>
#include <stdlib.h>
#include <math.h>
//based on http://www3.interscience.wiley.com/journal/121496529/abstract
using namespace minc;

void show_usage (const char * prog)
{
  std::cerr<<"Usage: "<<prog<<" <input.mnc> <output.mnc> [--step <f> --verbose ]"<<std::endl;
}

int main(int argc,char **argv)
{
  int verbose=0;
  int csv=0;
  int kappa=0,specificity=0,sensitivity=0;
  int jaccard=0;
  double _step=1.0;
  static struct option long_options[] = {
    {"verbose", no_argument,             &verbose, 1},
    {"quiet",   no_argument,             &verbose, 0},
    {"step",    required_argument,       0, 's'},
    {0, 0, 0, 0}
  };
  
  for (;;) {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "vq", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) break;

    switch (c)
    {
      case 0:
        break;
      case 'v':
        std::cout << "Version: 0.1" << std::endl;
        return 0;
      case 's':
        _step=atof(optarg);
        break;
      case '?':
        /* getopt_long already printed an error message. */
      default:
        show_usage (argv[0]);
        return 1;
    }
  }

  if((argc - optind) < 2) {
    show_usage (argv[0]);
    return 1;
  }
  
  try
  {
    minc_1_reader rdr;
    rdr.open(argv[optind],true);
    minc::minc_info new_info;

    //rdr.setup_read_float();
    simple_volume<float> input_vol,output_vol;
    load_simple_volume<float>(rdr,input_vol);
    new_info=rdr.info();
    
    minc::fixed_vec<3,float> old_step;
    minc::fixed_vec<3,float> old_start,new_start;
    minc::fixed_vec<3,int> new_len;
    
    for(int i=1;i<4;i++)
    {
      old_step[i-1]=new_info[rdr.map_space(i)].step;
      old_start[i-1]=new_info[rdr.map_space(i)].start;
      
      float len=(new_info[rdr.map_space(i)].length)*old_step[i-1];
      
      new_info[rdr.map_space(i)].start-=old_step[i-1];
      new_info[rdr.map_space(i)].step=_step;
      new_info[rdr.map_space(i)].start+=_step/2;
      new_start[i-1]=new_info[rdr.map_space(i)].start;
      
      new_len[i-1]=new_info[rdr.map_space(i)].length=ceil(fabs(len/_step));
    }
    
    output_vol.resize(new_len);
    
    for(int z=0;z<new_len[2];z++)
      for(int y=0;y<new_len[1];y++)
        for(int x=0;x<new_len[0];x++)
        {
          minc::fixed_vec<3,float> cc=IDX<float>(x*_step,y*_step,z*_step);
          
          cc+=new_start;
          cc/=old_step;
          cc-=old_start;
          
          output_vol.set(x,y,z,input_vol.interpolate(cc[0],cc[1],cc[2]));
        }
    minc_1_writer wrt;
    wrt.open(argv[optind+1],new_info,2,NC_FLOAT);
    save_simple_volume<float>(wrt,output_vol);
  } catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  return 0;
}
