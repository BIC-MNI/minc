#! /usr/bin/env perl
#
# Andrew Janke - rotor@cmr.uq.edu.au
# Center for Magnetic Resonance
# The University of Queensland
# http://www.cmr.uq.edu.au/~rotor
#
# Copyright Andrew Janke, The University of Queensland.
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies.  The
# author and the University of Queensland make no representations about the
# suitability of this software for any purpose.  It is provided "as is"
# without express or implied warranty.
#
# Inspired by Peter Neelins' (neelin@bic.mni.mcgill.ca) ana2mnc for ANALYZE 4.x.
# Since then it has grown into a veritable swiss-army chainsaw of 
# SPM, ANALYZE 7.5 and MINC conversion. 
#
# Wed Oct 18 09:29:44 EST 2000 - updated to Analyze 7.5 format
# Thu Oct 19 16:57:30 EST 2000 - Added support for writing out ANALYZE 7.5 headers
# Wed Oct 25 10:29:54 EST 2000 - Added mnc2ana, ana_show and ana_write functionality
# Wed Feb 13 16:22:05 EST 2002 - Added byte_swapping, THANKS PETER! 
# Thu Feb 28 13:38:03 EST 2002 - Removed dependencies on MNI::Perllib stuff

require 5.0;

use strict;
use warnings "all";
use Getopt::Tabular;

my($Help, $Usage, $me, @files, $history);
my(@opt_table, $clobber, $verbose, $settings, $args_h);
my($anabase, $anahdr, $anaimg, $mncfile, $matfile, $xfmfile);

$Help = <<HELP;
'ana2mnc' is a series of scripts.
It is designed for conversion between a number of formats:

   MINC:       a file format from the MNI
                  http://www.bic.mni.mcgill.ca/software
   ANALYZE:    a file format from the mayo clinic
                  http://www.mayo.edu/bir
   SPM:        a fMRI toolkit using a hybrid ANALYZE format
                  http://www.fil.ion.bpmf.ac.uk/spm

It is primarily designed for conversion of these file types to MINC, however
some reverse conversions are included. The available conversions are:

   ana2mnc     Convert a ANALYZE 7.5 .hdr .img couple to a MINC file
   mnc2ana     Convert a MINC file to a ANALYZE 7.5 .hdr .img couple 
   ana_show    Show the contents of a ANALYZE 7.5 .hdr file
   
   spm2mnc     Convert a SPM ANALYZE .hdr .img .mat tuple to a MINC file
   spm2xfm     Convert a SPM .mat transform to a MINC .xfm file
   spm_show    Show the contents of a SPM normalization .mat file

Problems or comments should be sent to: rotor\@cmr.uq.edu.au
HELP

chomp($me = `basename $0`);
$clobber = 0;
$verbose = 0;
$args_h  = create_analyze_header();
$settings = {};
@opt_table = (
     ["-verbose", "boolean", 0, \$verbose, 
        "be verbose"                                                    ],
     ["-clobber", "boolean", 0, \$clobber, 
        "clobber existing files"                                        ],
     
     ["Options for Conversion from analyze formats", "section"          ],
     ["-transverse", "const", "-transverse", \$settings->{orientation}, 
        "force files to be treated as transverse"                       ],
     ["-sagittal",   "const", "-sagittal",   \$settings->{orientation}, 
        "force files to be treated as sagittal"                         ],
     ["-coronal",    "const", "-coronal",    \$settings->{orientation}, 
        "force files to be treated as coronal"                          ],

#  these are currently commented out as possible additions in the future...
#  time will tell.
#
#      ["Options to override the analyze header", "section"],
#      ["-datatype",     "string",  1, \$args_h->{data_type},    "datatype"     ],
#      ["-db_name",      "string",  1, \$args_h->{db_name},      "db_name"      ],
#      ["-vox_units",    "string",  1, \$args_h->{vox_units},    "vox_units"    ],
#      ["-cal_units",    "string",  1, \$args_h->{cal_units},    "cal_units"    ],
#      ["-datatype",     "float",   1, \$args_h->{datatype},     "datatype"     ],
#      ["-bitpix",       "float",   1, \$args_h->{bitpix},       "bitpix"       ],
#      ["-vox_offset",   "float",   1, \$args_h->{vox_offset},   "vox_offset"   ],
#      ["-scale_factor", "float",   1, \$args_h->{scale_factor}, "scale_factor" ],
#      ["-cal_max",      "float",   1, \$args_h->{cal_max},      "cal_max"      ],
#      ["-cal_min",      "float",   1, \$args_h->{cal_min},      "cal_min"      ],
#      ["-compressed",   "float",   1, \$args_h->{compressed},   "compressed"   ],
#      ["-verified",     "float",   1, \$args_h->{verified},     "verified"     ],
#      ["-glmax",        "float",   1, \$args_h->{glmax},        "glmax"        ],
#      ["-glmin",        "float",   1, \$args_h->{glmin},        "glmin"        ],
#      ["-descrip",      "string",  1, \$args_h->{descrip},      "descrip"      ],
#      ["-aux_file",     "string",  1, \$args_h->{aux_file},     "aux_file"     ],
#      ["-orient",       "float",   1, \$args_h->{orient},       "orient"       ],
#      ["-generated",    "string",  1, \$args_h->{generated},    "generated"    ],
#      ["-scannum",      "string",  1, \$args_h->{scannum},      "scannum"      ],
#      ["-patient_id",   "string",  1, \$args_h->{patient_id},   "patient_id"   ],
#      ["-exp_date",     "string",  1, \$args_h->{exp_date},     "exp_date"     ],
#      ["-exp_time",     "string",  1, \$args_h->{exp_time},     "exp_time"     ],
#      ["-views",        "float",   1, \$args_h->{views},        "views"        ],
#      ["-vols_added",   "float",   1, \$args_h->{vols_added},   "vols_added"   ],
#      ["-start_field",  "float",   1, \$args_h->{start_field},  "start_field"  ],
#      ["-field_skip",   "float",   1, \$args_h->{field_skip},   "field_skip"   ],
#      ["-omax",         "float",   1, \$args_h->{omax},         "omax"         ],
#      ["-omin",         "float",   1, \$args_h->{omin},         "omin"         ],
#      ["-smax",         "float",   1, \$args_h->{smax},         "smax"         ],
#      ["-smin",         "float",   1, \$args_h->{smin},         "smin"         ],
     
     );

if   ($me eq "ana2mnc"){   @files = ('<in.hdr>', '<out.mnc>'); }
elsif($me eq "spm2mnc"){   @files = ('<in.hdr>', '<out.mnc>'); }
elsif($me eq "mnc2ana"){   @files = ('<in.mnc>', '<out.hdr>'); }
elsif($me eq "spm2xfm"){   @files = ('<in.mat>', '<out.xfm>'); }
elsif($me eq "ana_show"){  @files = ('<in.hdr>');              }
elsif($me eq "spm_show"){  @files = ('<in.mat>');              }

$Usage = "Usage: $me [options] @files\n".
         "       $me -help to list options\n\n";

# Check arguments
&Getopt::Tabular::SetHelp($Help, $Usage);
&GetOptions(\@opt_table, \@ARGV) || exit 1;
if($#ARGV != $#files){ die $Usage; }

# Get the history string
chomp($history = `date`);
$history .= '>>>> ' . join(' ', $me, @ARGV);

# then do whatever we are supposed to be doing
if($me eq "ana2mnc"){
   ($anabase, $mncfile) = @ARGV[0..1];
   $anabase =~ s/\.(hdr|img)$//;
   
   if(!-e "$anabase.hdr"){ 
      die "$me: Couldn't find $anabase.hdr\n"; 
      }
   if(!-e "$anabase.img"){ 
      die "$me: Couldn't find $anabase.img\n"; 
      }
   if(-e $mncfile && !$clobber){ 
      die "$me: $mncfile exists! use -clobber to overwrite\n"; 
      }

   # Read in the header file
   my($ana_hdr) = read_analyze_header("$anabase.hdr");
   if($verbose){ 
      print STDOUT dump_analyze_header($ana_hdr);
      }
   
   # convert to the general header format
   my($gen_hdr) = analyze_to_general_header($ana_hdr, $settings);
   if($verbose){
      print STDOUT dump_general_header($gen_hdr);
      }
   
   # check for a compressed img file
   if(!-e "$anabase.img" && -e "$anabase.img.gz"){
      system('gunzip', "$anabase.img.gz") ==  0 or die "$me: died during gunzip\n";
      }
   
   write_minc($gen_hdr, $mncfile, "$anabase.img");
   }

elsif($me eq "spm2mnc"){
   ($anabase, $mncfile) = @ARGV[0..1];
   $anabase =~ s/\.(hdr|img|mat)$//;
   
   if(!-e "$anabase.hdr"){ 
      die "$me: Couldn't find $anabase.hdr\n"; 
      }
   if(!-e "$anabase.img"){ 
      die "$me: Couldn't find $anabase.img\n"; 
      }
   if(!-e "$anabase.mat"){ 
      die "$me: Couldn't find $anabase.mat\n"; 
      }
   if(-e $mncfile && !$clobber){ 
      die "$me: $mncfile exists! use -clobber to overwrite\n"; 
      }

   # Read in the header file
   my($ana_hdr) = read_analyze_header("$anabase.hdr");
   if($verbose){ 
      print STDOUT dump_analyze_header($ana_hdr);
      }
   
   # convert to the general header format
   my($gen_hdr) = spm_to_general_header($ana_hdr, $settings);
   if($verbose){
      print STDOUT dump_general_header($gen_hdr);
      }
   
   # check for a compressed img file
   if(!-e "$anabase.img" && -e "$anabase.img.gz"){
      system('gunzip', "$anabase.img.gz") ==  0 or die "$me: died during gunzip\n";
      }
   
   write_minc($gen_hdr, $mncfile, "$anabase.img");
   }

elsif($me eq "mnc2ana"){
   ($mncfile, $anabase) = @ARGV[0..1];
   $anabase =~ s/\.(hdr|img)$//;
   
   if(!-e $mncfile){ 
      die "$me: Couldn't find $mncfile\n"; 
      }
   if((-e "$anabase.hdr" || -e "$anabase.img") && !$clobber){ 
      die "$me: $anabase.hdr or $anabase.img exist! use -clobber to overwrite\n"; 
      }
      
   my($h) = read_minc($mncfile);
   if($verbose){ print STDOUT dump_analyze_header($h); }
   write_analyze_header($h, "$anabase.hdr");
   
   # Set up mincextract command
   my($args) = "mincextract -normalize -positive_direction -filetype ";
   $args .= ($h->{bitpix} == 8) ? '-unsigned ' : '-signed ';
   $args .=  "$mncfile > $anabase.img\n";

   if($verbose){ print STDOUT $args; }
   system($args) == 0 or die "$me: died during mincextract command\n";
   }
   
elsif($me eq "spm2xfm"){
   ($matfile, $xfmfile) = @ARGV[0..1];
   chomp(my $date = `date`);
   
   my ($xfm_text, @dump, $c);
   
   if(!-e $matfile){ 
      die "$me: Couldn't find file: $matfile\n";
      }
   if(-e $xfmfile && !$clobber){ 
      die "$me: $xfmfile exists! use -clobber to overwrite\n"; 
      }
      
   # read in .mat file, convert to sensical format, remove first and last line
   @dump = `mat1dump $matfile`;
   for($c=1; $c<4; $c++){
      $dump[$c] =~ s/\t/\ /g;
      if($c == 3){ 
         $dump[$c] =~ s/\n/\;\n/g;
         }
      }
   
   open(SPM_XFM, ">$xfmfile"); 
   print SPM_XFM "MNI Transform File\n".
                 "%$date>>> Created by $me from $matfile\n\n".
                 "Transform_Type = Linear;\n".
                 "Linear_Transform =\n".
                 "@dump[1..3]";
   close(SPM_XFM);
   }
   
elsif($me eq "ana_show"){
   $anahdr = $ARGV[0];
   
   if(!-e "$anahdr"){
      die "$me: Couldn't find file: $anahdr\n"; 
      }
   print STDOUT dump_analyze_header(read_analyze_header($anahdr));
   }
   
elsif($me eq "spm_show"){
   $matfile = $ARGV[0];
   
   if(!-e $matfile){ 
      die "$me: Couldn't find $matfile\n"; 
      }
   print STDOUT `mat1dump $matfile`;
   }


# Subroutines #######################################################
# Unpack a value from a string (passed by reference)
# Legendary bit of code of peter neelins nicked from mri_to_minc 
sub destruct{
   my($stringref, $offset, $type, $bs) = @_;
    
   my(%Unpack_codes, $code, $number, $size, $tempstring, $iloop, $max);
    
   %Unpack_codes = ('a', 1,    'A', 1, 
                    's', 2,    'S', 2,
                    'i', 4,    'I', 4,
                    'f', 4,    'd', 8,
                   );
   
   # Check for byte swapping
   if($bs){
      if($type !~ /^\s*([a-zA-Z])(\d+)?\s*$/){
         die "$me: unrecognized data type \"$type\" on little-endian machine.\n";
         }
      $code = $1;
      $number = (defined($2) ? $2 : 1);
      if(!defined($Unpack_codes{$code})){
         die "$me: unrecognized unpack code \"$code\" on little-endian machine.\n";
         }
      $size = $Unpack_codes{$code};
      $tempstring = substr($$stringref, $offset, $number * $size);
      if($size > 1){
         $max = $number * $size;
         for($iloop=0; $iloop < $max; $iloop+=$size){
            substr($tempstring, $iloop, $size) = 
               reverse(substr($tempstring, $iloop, $size));
            }
         }
      return unpack("$type", $tempstring);
      }
       
    # No byte swapping required
   else{
      return unpack("x$offset $type", $$stringref);
      }
   }

sub analyze_to_general_header{
   my($ana_hdr, $settings) = @_;
   
   my($gen_hdr) = {};
   my(%ana_dtypes) = (   2  => ['-byte', '-unsigned'],
                         4  => ['-short', '-signed'],
                         8  => ['-long', '-signed'],
                        16 => ['-float'],
                        64 => ['-double'],
                       128 => ['-byte', '-unsigned', '-vector', '3']
                     );
                     
   my(%Orientation_to_world_order) = (
                                      '-transverse' => [0, 1, 2],
                                      '-sagittal'   => [2, 0, 1],
                                      '-coronal'    => [0, 2, 1],
                                      );
   
   # begin the conversions..
   if(!defined($ana_dtypes{$ana_hdr->{datatype}})) {
      die "$me: unknown data type:$ana_hdr->{data_type}\n";
      }
   $gen_hdr->{datatype} = $ana_dtypes{$ana_hdr->{datatype}};
   
   $gen_hdr->{voxel_min} = $ana_hdr->{glmin};
   $gen_hdr->{voxel_max} = $ana_hdr->{glmax};
   $gen_hdr->{real_min} = $ana_hdr->{cal_min};
   $gen_hdr->{real_max} = $ana_hdr->{cal_max};
   
   # get the dimension sizes
   $gen_hdr->{xsize} = @{$ana_hdr->{dim}}[1];
   $gen_hdr->{ysize} = @{$ana_hdr->{dim}}[2];
   $gen_hdr->{zsize} = @{$ana_hdr->{dim}}[3];
   $gen_hdr->{ntime} = @{$ana_hdr->{dim}}[4];
   
   # get orientation
   if(($ana_hdr->{orient} == 0) || ($ana_hdr->{orient} == 3)) {     # Transverse
      $gen_hdr->{orientation} = '-transverse';
      }
   elsif(($ana_hdr->{orient} == 2) || ($ana_hdr->{orient} == 5)) {  # Sagittal
      $gen_hdr->{orientation} = '-sagittal';
      }
   elsif(($ana_hdr->{orient} == 1) || ($ana_hdr->{orient} == 4)) {  # Coronal
      $gen_hdr->{orientation} = '-coronal';
      }
   else{                                                            # Unknown
      warn "$me: unknown data orientation: assuming transverse\n";
      $gen_hdr->{orientation} = '-transverse';
      }
   
   if(defined($settings->{orientation})){
      warn "$me: overriding file orientation with $settings->{orientation}\n";
      $gen_hdr->{orientation} = $settings->{orientation};
      }
   
   # get step info
   my(@steps) = @{$ana_hdr->{pixdim}}[1..3];
   my(@world_order) = @{$Orientation_to_world_order{$gen_hdr->{orientation}}};
   ($gen_hdr->{xstep}, $gen_hdr->{ystep}, $gen_hdr->{zstep}) = 
      @steps[@world_order];
   $gen_hdr->{xstep} *= -1;
   
   return $gen_hdr;
   }

sub spm_to_general_header{
   my($ana_hdr, $settings) = @_;
   
   my($gen_hdr) = {};
   my(%ana_dtypes) = (   2  => ['-byte', '-unsigned'],
                         4  => ['-short', '-signed'],
                         8  => ['-long', '-signed'],
                        16 => ['-float'],
                        64 => ['-double'],
                       128 => ['-byte', '-unsigned', '-vector', '3']
                     );
                     
   my(%Orientation_to_world_order) = (
                                      '-transverse' => [0, 1, 2],
                                      '-sagittal'   => [2, 0, 1],
                                      '-coronal'    => [0, 2, 1],
                                      );
   
   # begin the conversions..
   if(!defined($ana_dtypes{$ana_hdr->{datatype}})) {
      die "$me: unknown data type:$ana_hdr->{data_type}\n";
      }
   $gen_hdr->{datatype} = $ana_dtypes{$ana_hdr->{datatype}};
   
   $gen_hdr->{voxel_min} = $ana_hdr->{glmin};
   $gen_hdr->{voxel_max} = $ana_hdr->{glmax};
   $gen_hdr->{real_min} = $gen_hdr->{voxel_min}*$ana_hdr->{scale_factor};
   $gen_hdr->{real_max} = $gen_hdr->{voxel_max}*$ana_hdr->{scale_factor};
   
   # get the dimension sizes
   $gen_hdr->{xsize} = @{$ana_hdr->{dim}}[1];
   $gen_hdr->{ysize} = @{$ana_hdr->{dim}}[2];
   $gen_hdr->{zsize} = @{$ana_hdr->{dim}}[3];
   $gen_hdr->{ntime} = @{$ana_hdr->{dim}}[4];
   
   # get orientation
   if(($ana_hdr->{orient} == 0) || ($ana_hdr->{orient} == 3)) {     # Transverse
      $gen_hdr->{orientation} = '-transverse';
      }
   elsif(($ana_hdr->{orient} == 2) || ($ana_hdr->{orient} == 5)) {  # Sagittal
      $gen_hdr->{orientation} = '-sagittal';
      }
   elsif(($ana_hdr->{orient} == 1) || ($ana_hdr->{orient} == 4)) {  # Coronal
      $gen_hdr->{orientation} = '-coronal';
      }
   else{                                                            # Unknown
      warn "$me: unknown data orientation: assuming transverse\n";
      $gen_hdr->{orientation} = '-transverse';
      }
   
   if(defined($settings->{orientation})){
      warn "$me: overriding file orientation with $settings->{orientation}\n";
      $gen_hdr->{orientation} = $settings->{orientation};
      }
   
   # get step info
   my(@steps) = @{$ana_hdr->{pixdim}}[1..3];
   my(@world_order) = @{$Orientation_to_world_order{$gen_hdr->{orientation}}};
   ($gen_hdr->{xstep}, $gen_hdr->{ystep}, $gen_hdr->{zstep}) = 
      @steps[@world_order];
   $gen_hdr->{xstep} *= -1;
   
   # get origin information
   my(@starts) = @{$ana_hdr->{originator}}[0..2];
   @world_order = @{$Orientation_to_world_order{$gen_hdr->{orientation}}};
   ($gen_hdr->{xstart}, $gen_hdr->{ystart}, $gen_hdr->{zstart}) = 
      @starts[@world_order];
   
   $gen_hdr->{xstart} *= -$gen_hdr->{xstep};
   $gen_hdr->{ystart} *= -$gen_hdr->{ystep};
   $gen_hdr->{zstart} *= -$gen_hdr->{zstep};
   
   return $gen_hdr;
   }

# Read an analyze 7.5 header file and return a hash
sub read_analyze_header{
   open(HDR, $_[0]) or die "$me: error opening file: $_[0]\n";
   read(HDR, my($hdr_s), 40);
   read(HDR, my($dim_s), 108);
   read(HDR, my($his_s), 200);
   close(HDR);
   
   my($bs, $extents, $c, $h) = 0;
   
   # check if we need to byteswap
   $h->{sizeof_hdr} = destruct(\$hdr_s, 0,'i', 0);
   if($h->{sizeof_hdr} != 348){
      warn "$me: sizeof header: $h->{sizeof_hdr}\n";
      warn "$me: Hrm, attempting byte-swapping on $_[0]\n";
      $bs = 1;
      $h->{sizeof_hdr} = destruct(\$hdr_s, 0,'i', $bs);
      }
   
   if($h->{sizeof_hdr} != 348){
      warn "$me: sizeof header: $h->{sizeof_hdr}\n";
      die "$me: $_[0] doesn't appear to be a ANALYZE file\n".
           "    Who would really know though......\n";
      }
   
   if($verbose){ 
      print STDERR "sizeof_header: $h->{sizeof_hdr}  -- Byte Swap: $bs\n";
      }
   
   $h->{sizeof_hdr}    = destruct(\$hdr_s,  0,'i',   $bs);  #  4 - the byte size of the header file
   $h->{data_type}     = destruct(\$hdr_s,  4,'a10', $bs);  # 10 -
   $h->{db_name}       = destruct(\$hdr_s, 14,'a18', $bs);  # 18 -
   $h->{extents}       = destruct(\$hdr_s, 32,'i',   $bs);  #  4 - should be 16384
   $h->{session_error} = destruct(\$hdr_s, 36,'s',   $bs);  #  2 -
   $h->{regular}       = destruct(\$hdr_s, 38,'a',   $bs);  #  1 - 'r' indicates volumes are the same size
   $h->{hkey_un0}      = destruct(\$hdr_s, 39,'c',     0);  #  1 -
                                                            # 40 bytes
 
   $h->{dim}           =[destruct(\$dim_s,  0,'s8',  $bs)]; # 16 - array of the image dimensions
                                                            #        dim[0] # of dimensions; usually 4
                                                            #        dim[1]    X dim - pixels in an image row
                                                            #        dim[2]    Y dim - pixel rows in slice
                                                            #        dim[3]    Z dim - slices in a volume
                                                            #        dim[4] Time dim - volumes in database
   $h->{vox_units}     = destruct(\$dim_s, 16,'a4',  $bs);  #  4 - spatial units of measure for a voxel
   $h->{cal_units}     = destruct(\$dim_s, 20,'a8',  $bs);  #  8 - name of the calibration unit
   $h->{unused1}       = destruct(\$dim_s, 28,'s',   $bs);  #  2
   $h->{datatype}      = destruct(\$dim_s, 30,'s',   $bs);  #  2 - datatype for this image set
   $h->{bitpix}        = destruct(\$dim_s, 32,'s',   $bs);  #  2 - # of bits per voxel 1, 8, 16, 32, or 64.
   $h->{dim_un0}       = destruct(\$dim_s, 34,'s',   $bs);  #  2 -
   $h->{pixdim}        =[destruct(\$dim_s, 36,'f8',  $bs)]; # 32 - pixdim[] specifies the voxel dimensions:
                                                            #        pixdim[1] - voxel width
                                                            #        pixdim[2] - voxel height
                                                            #        pixdim[3] - interslice distance
                                                            #        ..etc
 
   $h->{vox_offset}    = destruct(\$dim_s, 68,'f',   $bs);  #  4 - byte offset in .img at which voxels start.
                                                            #        This value can be negative to specify
                                                            #        that the absolute value is applied for
                                                            #        every image
   $h->{scale_factor}  = destruct(\$dim_s, 72,'f',   $bs);  #  4 = funused1; scale factor used by SPM
   $h->{funused1}      = destruct(\$dim_s, 76,'f',   $bs);  #  4
   $h->{funused2}      = destruct(\$dim_s, 80,'f',   $bs);  #  4
   $h->{cal_max}       = destruct(\$dim_s, 84,'f',   $bs);  #  4 - calibrated max & min:
   $h->{cal_min}       = destruct(\$dim_s, 88,'f',   $bs);  #  4   www.mailbase.ac.uk/lists/spm/2000-09/0099.html
   $h->{compressed}    = destruct(\$dim_s, 92,'f',   $bs);  #  4
   $h->{verified}      = destruct(\$dim_s, 96,'f',   $bs);  #  4
   $h->{glmax}         = destruct(\$dim_s,100,'i',   $bs);  #  4 - global max | pixel values
   $h->{glmin}         = destruct(\$dim_s,104,'i',   $bs);  #  4 - global min | (entire database)
                                                            #108 bytes
 
   $h->{descrip}       = destruct(\$his_s,  0,'a80', $bs);  # 80
   $h->{aux_file}      = destruct(\$his_s, 80,'a24', $bs);  # 24
   $h->{orient}        = destruct(\$his_s,104,'c',     0);  #  1 - slice orientation           | disregarded
                                                            #        0 - transverse unflipped  | by SPM
                                                            #        1 - coronal    unflipped  | instead a
                                                            #        2 - sagittal   unflipped  | .mat file is
                                                            #        3 - transverse flipped    | written out
                                                            #        4 - coronal    flipped    | with this
                                                            #        5 - sagittal   flipped    | information
   $h->{originator}    =[destruct(\$his_s,105,'s5',  $bs)]; # 10 - origin
                                                            #        originator[0] x-origin    | non standard
                                                            #        originator[1] y-origin    | SPM use only
                                                            #        originator[2] z-origin    |
   $h->{generated}     = destruct(\$his_s,115,'a10', $bs);  # 10
   $h->{scannum}       = destruct(\$his_s,125,'a10', $bs);  # 10
   $h->{patient_id}    = destruct(\$his_s,135,'a10', $bs);  # 10
   $h->{exp_date}      = destruct(\$his_s,145,'a10', $bs);  # 10
   $h->{exp_time}      = destruct(\$his_s,155,'a10', $bs);  # 10
   $h->{hist_un0}      = destruct(\$his_s,165,'a3',  $bs);  #  3
   $h->{views}         = destruct(\$his_s,168,'i',   $bs);  #  4
   $h->{vols_added}    = destruct(\$his_s,172,'i',   $bs);  #  4
   $h->{start_field}   = destruct(\$his_s,176,'i',   $bs);  #  4
   $h->{field_skip}    = destruct(\$his_s,180,'i',   $bs);  #  4
   $h->{omax}          = destruct(\$his_s,184,'i',   $bs);  #  4
   $h->{omin}          = destruct(\$his_s,188,'i',   $bs);  #  4
   $h->{smax}          = destruct(\$his_s,192,'i',   $bs);  #  4
   $h->{smin}          = destruct(\$his_s,196,'i',   $bs);  #  4
                                                            #200 bytes
   return $h;
   }

# return a ASCII dump of a general header
sub dump_general_header{
   my($h) = shift;
   my($tmp);
   
   $tmp = "General Header\n";
   foreach (sort(keys(%{$h}))){
      if($h->{$_} =~ /ARRAY/){
         $tmp .= " $_\t<". join(' ' , @{$h->{$_}}) . ">\n";
         }
      else{
         $tmp .= " $_\t<$h->{$_}>\n";
         }
      }
   $tmp .= "\n";
      
   return $tmp;
   }

# return a ASCII dump of an ANALYZE 7.5 header
sub dump_analyze_header{
   my($h) = shift;
   
   return
      "HDR:\n".
         " sizeof_hdr     <$h->{sizeof_hdr}>\n".
         " data_type      <$h->{data_type}>\n".
         " db_name        <$h->{db_name}>\n".
         " extents        <$h->{extents}>\n".
         " session_error  <$h->{session_error}>\n".
         " regular        <$h->{regular}>\n".
         " hkey_un0       <$h->{hkey_un0}>\n". 
   
      "DIMENSION:\n".
         " dim:           <" . join(' ', @{$h->{dim}}) . ">\n".
         " vox_units      <$h->{vox_units}>\n".
         " cal_units      <$h->{cal_units}>\n".
         " unused1        <$h->{unused1}>\n".
         " datatype       <$h->{datatype}>\n".
         " bitpix         <$h->{bitpix}>\n".
         " dim_un0        <$h->{dim_un0}>\n".
         " pixdim         <" . join(' ', @{$h->{pixdim}}) . ">\n".
         " vox_offset     <$h->{vox_offset}>\n".
         " scale_factor   <$h->{scale_factor}>\n".
         " funused1       <$h->{funused1}>\n".
         " funused2       <$h->{funused2}>\n".
         " cal_max        <$h->{cal_max}>\n".
         " cal_min        <$h->{cal_min}>\n".
         " compressed     <$h->{compressed}>\n".
         " verified       <$h->{verified}>\n".
         " glmax          <$h->{glmax}>\n".
         " glmin          <$h->{glmin}>\n". 
   
      "HISTORY:\n".
         " descrip        <$h->{descrip}>\n".
         " aux_file       <$h->{aux_file}>\n".
         " orient         <$h->{orient}>\n".
         " originator     <" . join(' ', @{$h->{originator}}) . ">\n".
         " generated      <$h->{generated}>\n".
         " scannum        <$h->{scannum}>\n".
         " patient_id     <$h->{patient_id}>\n".
         " exp_date       <$h->{exp_date}>\n".
         " exp_time       <$h->{exp_time}>\n".
         " hist_un0       <$h->{hist_un0}>\n".
         " views          <$h->{views}>\n".
         " vols_added     <$h->{vols_added}>\n".
         " start_field    <$h->{start_field}>\n".
         " field_skip     <$h->{field_skip}>\n".
         " omax           <$h->{omax}>\n".
         " omin           <$h->{omin}>\n".
         " smax           <$h->{smax}>\n".
         " smin           <$h->{smin}>\n".
         "\n";  
   }

# Write an ANALYZE 7.5 header to a file
sub write_analyze_header{
   my($h, $hdrfile) = @_;
   
   my($hdr_s, $dim_s, $his_s);
   
   $hdr_s  = pack("i",    $h->{sizeof_hdr}          );
   $hdr_s .= pack("a10",  $h->{data_type}           );
   $hdr_s .= pack("a18",  $h->{db_name}             );
   $hdr_s .= pack("i",    $h->{extents}             );
   $hdr_s .= pack("s",    $h->{session_error}       );
   $hdr_s .= pack("a",    $h->{regular}             );
   $hdr_s .= pack("c",    $h->{hkey_un0}            );
 
   $dim_s  = pack("s8",   @{$h->{dim}}[0..7]        );
   $dim_s .= pack("a4",   $h->{vox_units}           );
   $dim_s .= pack("a8",   $h->{cal_units}           );
   $dim_s .= pack("s",    $h->{unused1}             );
   $dim_s .= pack("s",    $h->{datatype}            );
   $dim_s .= pack("s",    $h->{bitpix}              );
   $dim_s .= pack("s",    $h->{dim_un0}             );
   $dim_s .= pack("f8",   @{$h->{pixdim}}[0..7]     );
   $dim_s .= pack("f",    $h->{vox_offset}          );
   $dim_s .= pack("f",    $h->{scale_factor}        );
   $dim_s .= pack("f",    $h->{funused1}            );
   $dim_s .= pack("f",    $h->{funused2}            );
   $dim_s .= pack("f",    $h->{cal_max}             );
   $dim_s .= pack("f",    $h->{cal_min}             );
   $dim_s .= pack("f",    $h->{compressed}          );
   $dim_s .= pack("f",    $h->{verified}            );
   $dim_s .= pack("i",    $h->{glmax}               );
   $dim_s .= pack("i",    $h->{glmin}               );
 
   $his_s  = pack("a80",  $h->{descrip}             );
   $his_s .= pack("a24",  $h->{aux_file}            );
   $his_s .= pack("c",    $h->{orient}              );
   $his_s .= pack("s5",   @{$h->{originator}}[0..4] );
   $his_s .= pack("a10",  $h->{generated}           );
   $his_s .= pack("a10",  $h->{scannum}             );
   $his_s .= pack("a10",  $h->{patient_id}          );
   $his_s .= pack("a10",  $h->{exp_date}            );
   $his_s .= pack("a10",  $h->{exp_time}            );
   $his_s .= pack("a3",   $h->{hist_un0}            );
   $his_s .= pack("i",    $h->{views}               );
   $his_s .= pack("i",    $h->{vols_added}          );
   $his_s .= pack("i",    $h->{start_field}         );
   $his_s .= pack("i",    $h->{field_skip}          );
   $his_s .= pack("i",    $h->{omax}                );
   $his_s .= pack("i",    $h->{omin}                );
   $his_s .= pack("i",    $h->{smax}                );
   $his_s .= pack("i",    $h->{smin}                );
   
   open(HDR, ">$hdrfile") or die "$me: error opening file: $hdrfile\n";
   syswrite(HDR, $hdr_s, 40);
   syswrite(HDR, $dim_s, 108);
   syswrite(HDR, $his_s, 200);
   close(HDR);
   }
   
# Create and return an ANALYZE 7.5 as a hash
sub create_analyze_header{
   my($h) = {};
   $h->{sizeof_hdr}    = 348;    $h->{data_type}     = 0;
   $h->{db_name}       = '';     $h->{extents}       = 16384;
   $h->{session_error} = 0;      $h->{regular}       = 'r';
   $h->{hkey_un0}      = 0;
   
   $h->{dim}           = [0,0,0,0,0,0,0,0];
   $h->{vox_units}     = '';     $h->{cal_units}     = '';
   $h->{unused1}       = 0;      $h->{datatype}      = 0;
   $h->{bitpix}        = 0;      $h->{dim_un0}       = 0;
   $h->{pixdim}        = [0,0,0,0,0,0,0,0];
   $h->{vox_offset}    = 0;      $h->{scale_factor}  = 1.0;
   $h->{funused1}      = 0;      $h->{funused2}      = 0;
   $h->{cal_max}       = 0;      $h->{cal_min}       = 0;
   $h->{compressed}    = 0;      $h->{verified}      = 0;
   $h->{glmax}         = 0;      $h->{glmin}         = 0;
   
   $h->{descrip}       = '';     $h->{aux_file}      = '';
   $h->{orient}        = 0;      $h->{originator}    = [0,0,0,0,0];
   $h->{generated}     = '';     $h->{scannum}       = '';
   $h->{patient_id}    = '';     $h->{exp_date}      = '';
   $h->{exp_time}      = '';     $h->{hist_un0}      = '';
   $h->{views}         = 0;      $h->{vols_added}    = 0;
   $h->{start_field}   = 0;      $h->{field_skip}    = 0;
   $h->{omax}          = 0;      $h->{omin}          = 0;
   $h->{smax}          = 0;      $h->{smin}          = 0;
   return $h;
   }

# Write out a MINC file based upon a given ANALYZE 7.5 header and a raw data file
sub write_minc{
   my($gen_hdr, $mncfile, $rawfile) = @_;

   # Set up rawtominc command
   my(@args) = ('rawtominc');
   if($clobber){ push(@args, '-clobber'); }
   
   # datatype and ranges
   push(@args, @{$gen_hdr->{datatype}});
   push(@args, '-range', $gen_hdr->{voxel_min}, $gen_hdr->{voxel_max});
   if($gen_hdr->{real_min} < $gen_hdr->{real_max}){
      push(@args, '-real_range', $gen_hdr->{real_min}, $gen_hdr->{real_max});
      }
   else{
      push(@args, '-scan_range');
      }
   
   # orientation and step information
   push(@args, $gen_hdr->{orientation});
   push(@args, '-xstep', $gen_hdr->{xstep},
               '-ystep', $gen_hdr->{ystep}, 
               '-zstep', $gen_hdr->{zstep});
   
   # if no start info center on the volume
   if(!defined($gen_hdr->{xstart})){
      warn "$me: No x origin info found, guessing..\n";
      $gen_hdr->{xstart} = -$gen_hdr->{xsize}/2*$gen_hdr->{xstep};
      }
   if(!defined($gen_hdr->{ystart})){
      warn "$me: No y origin info found, guessing..\n";
      $gen_hdr->{ystart} = -$gen_hdr->{ysize}/2*$gen_hdr->{ystep};
      }
   if(!defined($gen_hdr->{zstart})){
      warn "$me: No z origin info found, guessing..\n";
      $gen_hdr->{zstart} = -$gen_hdr->{zsize}/2*$gen_hdr->{zstep};
      }
   
   push(@args, '-xstart', $gen_hdr->{xstart},
               '-ystart', $gen_hdr->{ystart},
               '-zstart', $gen_hdr->{zstart});
   
   # history
   push(@args, '-sattribute', ":history='$history'");


   # files and sizes
   push(@args, '-input', $rawfile, $mncfile);
   if($gen_hdr->{ntime} > 1){
      push(@args, $gen_hdr->{ntime});
      }
   push(@args, @{$gen_hdr}{'zsize','ysize','xsize'});

   if($verbose){ print STDOUT "@args\n"; }
   system(@args) == 0 or die "$me: died during rawtominc system command\n";
   }

# Read a minc file header and return an appropriate ANALYZE 7.5 header
sub read_minc{
   my($mncfile) = shift;
   my(@data, $args, @start, @step, @length, @dims);
   
   my($h) = create_analyze_header();
#   ($h->{cal_min}, $h->{cal_max}) = volume_minmax($mncfile);
   
   $h->{glmin} = 0;
   $h->{glmax} = 65535;
   
   $args = "mincinfo -error_string 0 ".
           "-attval xspace:start -attval yspace:start -attval zspace:start ".
           "-attval xspace:step -attval yspace:step -attval zspace:step ".
           "-dimlength xspace -dimlength yspace -dimlength zspace ".
           "-dimnames $mncfile";
   
   @data = split(/\n/, `$args`);
   
   @start  = @data[0..2];
   @step   = @data[3..5];
   @length = @data[6..8];
   @dims   = split(" ", $data[9]);
   
   print "Start:  ". join(" - ", @start) . "\n";
   print "Step:   ". join(" - ", @step) . "\n";
   print "Length: ". join(" - ", @length) . "\n";
   print "Dims:   ". join(" - ", @dims) . "\n";
   
   my $c = 0;
   $h->{dim}[$c] = $h->{pixdim}[$c] = $#dims + 2;
   foreach (@dims){
      $h->{dim}[$c+1]    = $length[$c];
      $h->{pixdim}[$c+1] = abs($step[$c]);
      
      $c ++;
      }
   
   # contains: 0 corresponding ANALYZE datatype
   #           1 corresponding ANALYZE bitpix
   #           2 min
   #           3 max 
   my(%dtypes) = ( 'byte'   => [  2,  8,      0,   256],   # unsigned
                   'short'  => [  4, 16, -32767, 32767],   # signed
                   'long'   => [  8, 32, ],                # signed
                   'float'  => [ 16, 32, ],
                   'double' => [ 64, 64, ],
                   'vector' => [128, 24, ],
                 );
   
   chomp(my $dt   = `mincinfo -vartype image $mncfile`);
   $h->{datatype} = $dtypes{$dt}[0];
   $h->{bitpix}   = $dtypes{$dt}[1];
   
   return $h;
   }

