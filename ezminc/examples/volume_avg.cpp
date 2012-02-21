/* ----------------------------- MNI Header -----------------------------------
@NAME       : volume_avg
@DESCRIPTION: an example of calculating volume average
@COPYRIGHT  :
              Copyright 2009 Vladimir Fonov, McConnell Brain Imaging Centre, 
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
#include <math.h>

using namespace minc;

void show_usage(const char *name)
{
  std::cerr 
	  << "Usage: "<<name<<" <input1> .... <inputn>  <output> " << std::endl
    << "\tn should be more than 1"<< std::endl
    << "Optional parameters:" << std::endl
    << "\t--verbose be verbose" << std::endl
    << "\t--clobber clobber the output files" << std::endl
    << "\t--sd <sd.mnc> "<< std::endl;
}

int main(int argc,char **argv)
{
  int clobber=0;
  int verbose=0;
  std::string sd_f;
  
	static struct option long_options[] =
  {
    {"verbose", no_argument, &verbose, 1},
    {"quiet", no_argument, &verbose, 0},
    {"clobber", no_argument, &clobber, 1},
    {"sd", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };
  
	int c;
	for (;;)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "s", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			break;
    case 's':
      sd_f=optarg;
      break;
		case '?':
			/* getopt_long already printed an error message. */
		default:
			show_usage(argv[0]);
			return 1;
		}
	}

	if ((argc - optind) < 3)
	{
		show_usage(argv[0]);
		return 1;
	}
  
  std::string output=argv[argc-1]; //last argument is output file... maybe we should make it a parameter instead?
  argc-=optind;
  
  if(!clobber && !access (output.c_str(), F_OK))
  {
    std::cerr << output.c_str () << " Exists!" << std::endl;
    return 1;
  }

	try
  {
    
    minc_1_reader rdr1;
    rdr1.open(argv[optind]);
    minc_float_volume _avg;
    
    load_simple_volume<float>(rdr1,_avg);
    
    minc_float_volume _sd(_avg);
    minc_float_volume _tmp(_avg);
    

    for(int i=0;i<_avg.c_buf_size();i++)
    {
      _sd.c_buf()[i]=_avg.c_buf()[i]*_avg.c_buf()[i];
    }
    
    for(int i=1;i<(argc-1);i++)
    {
      minc_1_reader rdr2;
      rdr2.open(argv[optind+i]);
      if(!is_same(rdr1,rdr2))
      {
        return 1;
      }
      
      load_simple_volume<float>(rdr2,_tmp);
      _avg+=_tmp;
      _tmp*=_tmp;
      _sd+=_tmp;
    }
    _avg/=(float)(argc-1);
    for(int i=0;i<_avg.c_buf_size();i++)
    {
      _sd.c_buf()[i]=sqrt(_sd.c_buf()[i]/(argc-1) - _avg.c_buf()[i]*_avg.c_buf()[i] );
    }
    minc_1_writer wrt;
    wrt.open(output.c_str(),rdr1.info(),2,NC_FLOAT);
    save_simple_volume<float>(wrt,_avg);
    
    if(!sd_f.empty())
    {
      minc_1_writer wrt2;
      wrt2.open(sd_f.c_str(),rdr1.info(),2,NC_FLOAT);
      save_simple_volume<float>(wrt2,_sd);
    }
    
	} catch (const minc::generic_error & err) {
    std::cerr << "Got an error at:" << err.file () << ":" << err.line () << std::endl;
    std::cerr << err.msg()<<std::endl;
    return 1;
  }
  
  return 0;
}

