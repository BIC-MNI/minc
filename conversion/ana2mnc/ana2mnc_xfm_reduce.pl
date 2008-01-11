#! /usr/bin/env perl
#
# Andrew Janke - a.janke@gmail.com
# Center for Magnetic Resonance
# The University of Queensland
#
# MINC is a free file format from the MNI: www.bic.mni.mcgill.ca
# ANALYZE is a commercial file format from the mayo clinic: www.mayo.edu/bir
# SPM is a free fMRI toolkit using a hybrid ANALYZE format: www.fil.ion.bpmf.ac.uk/spm
#
# Inspired by Peter Neelins' (neelin@bic.mni.mcgill.ca) ana2mnc for ANALYZE 4.x.
# Since then it has grown into a veritable swiss-army chainsaw of 
# SPM, ANALYZE 7.5 and MINC conversion. 
#
# The reading and transformation of SPM .mat files to minc .xfm files is
#    Still somewhat experimental:  The following is from spm_realign.m
#    x1 = M(1,1)*x0 + M(1,2)*y0 + M(1,3)*z0 + M(1,4)
#    y1 = M(2,1)*x0 + M(2,2)*y0 + M(2,3)*z0 + M(2,4)
#    z1 = M(3,1)*x0 + M(3,2)*y0 + M(3,3)*z0 + M(3,4)
# This is equivalent to the MINC version (if you dig enough in the MINC 
#    distribution you will find a file volume_io/Geometry/transforms.c)
#    in the function: homogenous_transform_point
# The actual matrix decomposing is analagous to Louis Collins's 
#    extract_parameters_from_matrix in minctracc/Numerical/make_rots.c
#    in mni_autoreg package
#
# It can be invoked as:
#    ana2mnc   - convert ANALYZE 7.5 to MINC
#    mnc2ana   - convert MINC to ANALYZE 7.5
#    ana_show  - Show the contents of an ANALYZE 7.5 header
#    ana_write - write an ANALYZE header (requires gobs of additional CL arguments)
#
# Wed Oct 18 09:29:44 EST 2000 - updated to Analyze 7.5 format
# Thu Oct 19 16:57:30 EST 2000 - Added support for writing out ANALYZE 7.5 headers
# Wed Oct 25 10:29:54 EST 2000 - Added mnc2ana, ana_show and ana_write functionality
# Tue Nov 28 15:58:57 EST 2000 - Completed transformation decomposition added xfm_reduce

require 5.0;

use strict;
use warnings "all";

use Math::Trig;
use Math::MatrixReal;
use MNI::Startup qw(nocputimes);
use MNI::MincUtilities qw(:range :geometry);
use Getopt::Tabular;

my($clobber) = '';
my($verbose) = 0;
my($spm)     = 0;
my($args_h)  = create_analyze_header();

my(@opt_table) = (
              ["-verbose", "boolean",  0,         \$verbose, "Be Verbose"],
              ["-clobber", "const",   "-clobber", \$clobber, "clobber existing files"],
              ["-spm",     "boolean",  0,         \$spm,     "Read in the SPM .mat as well"],
              
              
              $h->{data_type}     = 0;
              $h->{db_name}       = '';
              $h->{regular}       = 'r';
              $h->{hkey_un0}      = 0;
 
              $h->{dim}           = [0,0,0,0,0,0,0,0];
              $h->{vox_units}     = '';
              $h->{cal_units}     = '';
              $h->{unused1}       = 0;
              $h->{datatype}      = 0;
              $h->{bitpix}        = 0;
              $h->{dim_un0}       = 0;
              $h->{pixdim}        = [0,0,0,0,0,0,0,0];
              $h->{vox_offset}    = 0;
              $h->{scale_factor}  = 1.0;
              $h->{funused1}      = 0;
              $h->{funused2}      = 0;
              $h->{cal_max}       = 0;
              $h->{cal_min}       = 0;
              $h->{compressed}    = 0;
              $h->{verified}      = 0;
              $h->{glmax}         = 0;
              $h->{glmin}         = 0;
 
              $h->{descrip}       = '';
              $h->{aux_file}      = '';
              $h->{orient}        = 0;
              $h->{originator}    = [0,0,0,0,0];
              $h->{generated}     = '';
              $h->{scannum}       = '';
              $h->{patient_id}    = '';
              $h->{exp_date}      = '';
              $h->{exp_time}      = '';
              $h->{hist_un0}      = '';
              $h->{views}         = 0;
              $h->{vols_added}    = 0;
              $h->{start_field}   = 0;
              $h->{field_skip}    = 0;
              $h->{omax}          = 0;
              $h->{omin}          = 0;
              $h->{smax}          = 0;
              $h->{smin}          = 0;
   }
   
                 );

chomp(my($prog) = `basename $0`);

my $files; my $nfiles;
if    ($prog eq "ana2mnc"){    $files = "<in.hdr> <out.mnc>"; $nfiles = 2; }
elsif ($prog eq "mnc2ana"){    $files = "<in.mnc> <out.hdr>"; $nfiles = 2; }
elsif ($prog eq "ana_show"){   $files = "<in.hdr>";           $nfiles = 1; }
elsif ($prog eq "ana_write"){  $files = "<out.hdr>";          $nfiles = 1; }
elsif ($prog eq "spm_show"){   $files = "<in.mat>";           $nfiles = 1; }
elsif ($prog eq "spm_conv"){   $files = "<in.mat>";           $nfiles = 1; }
elsif ($prog eq "xfm_reduce"){ $files = "<in.xfm>";           $nfiles = 1; }

my($Usage) = "Usage: $prog [options] $files\n".
             "$prog -help to list options\n";

my($Help) = <<HELP;
This script can be invoked as:
   ana2mnc     Convert a ANALYZE 7.5 .hdr .img couple to a MINC file
   mnc2ana     Convert a MINC file to a ANALYZE 7.5 .hdr .img couple 
   ana_show    Show the contents of a ANALYZE 7.5 .hdr file
   ana_write   Write a ANALYZE .hdr file from input arguments
   spm_show    Show the contents of a SPM normalization .mat file
   spm_conv    Convert a SPM normalization .mat to a .xfm file
   xfm_reduce  Reduce a MNI .xfm file to it least form
HELP

# Check arguments
&Getopt::Tabular::SetHelp ($Help, $Usage);
&GetOptions (\@opt_table, \@ARGV) || exit 1;
if ($#ARGV < $nfiles - 1){ die $Usage; }
my($mncfile, $anafile, $xfmfile);

if ($prog eq "ana2mnc"){
   ($anafile, $mncfile) = @ARGV[0..1];
   $anafile =~ s/\....$//;
   
   if (!-e "$anafile.hdr" || !-e "$anafile.img"){ 
      die "$prog: Couldn't find $anafile.hdr and $anafile.img\n"; 
      }
   if (-e $mncfile && ($clobber ne "-clobber")){ 
      die "$prog: $mncfile exists! use -clobber to overwrite\n"; 
      }

   # Read in the header file
   my($h) = read_analyze_header("$anafile.hdr");
   if ($verbose){ print STDOUT dump_analyze_header($h); }
   write_minc($h, $mncfile, "$anafile.img");
   }

elsif ($prog eq "mnc2ana"){
   ($mncfile, $anafile) = @ARGV[0..1];
   $anafile =~ s/\....$//;
   
   if (!-e $mncfile){ 
      die "$prog: Couldn't find $mncfile\n"; 
      }
   if ((-e "$anafile.hdr" || -e "$anafile.img") && ($clobber ne "-clobber")){ 
      die "$prog: $anafile.hdr or $anafile.img exist! use -clobber to overwrite\n"; 
      }
      
   my($h) = read_minc($mncfile);
   if ($verbose){ print STDOUT dump_analyze_header($h); }
   write_analyze_header($h, "$anafile.hdr");
   
   # Set up mincextract command
   my $args = "mincextract -normalize -positive_direction -filetype ";
   $args   .= ($h->{bitpix} == 8) ? '-unsigned ' : '-signed ';
   $args   .= "$mncfile > $anafile.img\n";

   if ($verbose){ print STDOUT $args; }
   system($args) == 0 or die "Died during mincextract command\n";
   }
   
elsif ($prog eq "ana_show"){
   $anafile = $ARGV[0];
   $anafile =~ s/\....$//;
   
   if (!-e "$anafile.hdr"){ die "$prog: Couldn't find $anafile.hdr\n"; }
   print STDOUT dump_analyze_header(read_analyze_header("$anafile.hdr"));
   }
   
elsif ($prog eq "ana_write"){
   $anafile = $ARGV[0];
   $anafile =~ s/\....$//;
   
   if (-e "$anafile.hdr" && ($clobber ne "-clobber")){ 
      die "$prog: $anafile.hdr exists! use -clobber to overwrite\n"; 
      }
      
   my($h) = create_analyze_header();
   write_analyze_header($h, "$anafile.hdr");
   }
   
elsif ($prog eq "spm_show"){
   $anafile = $ARGV[0];
   $anafile =~ s/\....$//;
   
   if (!-e "$anafile.mat"){ die "$prog: Couldn't find $anafile.mat\n"; }
   print STDOUT `mat1dump $anafile.mat`;
   }
   
elsif ($prog eq "spm_conv"){
   $anafile = $ARGV[0];
   $anafile =~ s/\....$//;
   chomp(my $date = `date`);
   
   my($M, @Trans, @Scale, @Shear, @Rotn);
   
   if (!-e "$anafile.mat"){ die "$prog: Couldn't find $anafile.mat\n"; }
   
   # perl line noise to read in a text file Matrix into MatrixReal
   my @matdump = `mat1dump $anafile.mat`;
   $M = Math::MatrixReal->new_from_string("[ " . join(" ]\n[ ", @matdump[1..4]) . " ]\n");
   if ($verbose){ print STDERR "M:\n$M"; }
   
   decompose_transformation_matrix($M, \@Trans, \@Scale, \@Shear, \@Rotn);
   print_transformation_matrix($M, "M");
   
   my (@resliceR, $sign);
   my $c = 0;
   foreach (@Rotn){ 
      $resliceR[$c] = 0;
      
      $sign = $_/abs($_);
      
      if (abs($_) > pi()){ die "$prog: Bugger! this angle is too big!: $_\n"; }
      
      if (abs($_) > pi()/2){   # if 90 deg
         $Rotn[$c]     -= $sign * pi()/2;
         $resliceR[$c] += $sign * pi()/2;
         }
         
      if (abs($_) > pi()/4){   # if 45 deg reverse the rotation (subtract 90deg).
         $Rotn[$c]     -= $sign * pi()/2;
         $resliceR[$c] += $sign * pi()/2;
         }

      $c ++;
      }
   
   my $RESLICE = create_transformation_matrix(undef, undef, undef, \@resliceR);
   print_transformation_matrix($RESLICE, "RESLICE");
   
   my $MINCxfm = create_transformation_matrix(\@Trans, \@Scale, \@Shear, \@Rotn);
   $MINCxfm = $M * $RESLICE->decompose_LR()->invert_LR();
   
   print_transformation_matrix($MINCxfm, "MINCxfm");
   print_transformation_matrix($MINCxfm * $RESLICE, "CHECK");
   
   
   open(SPM_XFM, ">$anafile.spm_xfm"); 
   print SPM_XFM "SPM Transform File\n".
                 "%$date>>> Created by $prog from $anafile.mat\n\n".
                 "@matdump[1..3]";
   close(SPM_XFM);
   }
   
elsif ($prog eq "xfm_reduce"){
   $xfmfile = $ARGV[0];
   
   my($Mtext, $M, @Trans, @Scale, @Shear, @Rotn);
   
   if (!-e "$xfmfile"){ die "$prog: Couldn't find $xfmfile\n"; }
   
   # perl line noise to read in a text file Matrix into MatrixReal
   my @MNIdump = `cat $xfmfile`;
   
   my $line;
   do{ $line = shift(@MNIdump) or die "$prog: Couldn't find a MNI Transform in: $xfmfile\n";} 
   until $line =~ m/Linear_Transform/;
   
   foreach (@MNIdump[0..2]){
      chomp;
      s/\;//;
      $Mtext .= "[ $_ ]\n";
      }
   $Mtext .= "[ 0 0 0 1 ]\n";
   
   $M = Math::MatrixReal->new_from_string($Mtext);
   decompose_transformation_matrix($M, \@Trans, \@Scale, \@Shear, \@Rotn);
   
   my $c = 0;
   foreach (@Rotn){
      $Rotn[$c] = rad2deg($_);
      $c ++;
      }
   
   printf("Translation: %10.5f %10.5f %10.5f\n", @Trans);
   printf("Rotation:    %10.5f %10.5f %10.5f\n", @Rotn);
   printf("Scale:       %10.5f %10.5f %10.5f\n", @Scale);
   printf("Shear:       %10.5f %10.5f %10.5f\n", @Shear);
   }

# Subroutines #######################################################

# Unpack a value from a string (passed by reference)
sub destruct{
   my($stringref, $offset, $type) = @_;
   return unpack("x$offset $type", $$stringref);
   }

# Read an analyze 7.5 header file and return a hash
sub read_analyze_header{
   open(HDR, $_[0]) or die "Error opening file: $_[0]\n";
   read(HDR, my($hdr_s), 40);
   read(HDR, my($dim_s), 108);
   read(HDR, my($his_s), 200);
   close(HDR);
   
   my($h) = {};
   $h->{sizeof_hdr}    = destruct(\$hdr_s,  0,'i'  );  #  4 - the byte size of the header file
   $h->{data_type}     = destruct(\$hdr_s,  4,'a10');  # 10 -
   $h->{db_name}       = destruct(\$hdr_s, 14,'a18');  # 18 -
   $h->{extents}       = destruct(\$hdr_s, 32,'i'  );  #  4 - should be 16384
   $h->{session_error} = destruct(\$hdr_s, 36,'s'  );  #  2 -
   $h->{regular}       = destruct(\$hdr_s, 38,'a'  );  #  1 - 'r' indicates volumes are the same size
   $h->{hkey_un0}      = destruct(\$hdr_s, 39,'c'  );  #  1 -
                                                       # 40 bytes
   
   $h->{dim}           =[destruct(\$dim_s,  0,'s8' )]; # 16 - array of the image dimensions
                                                       #        dim[0] # of dimensions; usually 4
                                                       #        dim[1]    X dim - pixels in an image row
                                                       #        dim[2]    Y dim - pixel rows in slice
                                                       #        dim[3]    Z dim - slices in a volume
                                                       #        dim[4] Time dim - volumes in database
   $h->{vox_units}     = destruct(\$dim_s, 16,'a4' );  #  4 - spatial units of measure for a voxel
   $h->{cal_units}     = destruct(\$dim_s, 20,'a8' );  #  8 - name of the calibration unit
   $h->{unused1}       = destruct(\$dim_s, 28,'s'  );  #  2
   $h->{datatype}      = destruct(\$dim_s, 30,'s'  );  #  2 - datatype for this image set
   $h->{bitpix}        = destruct(\$dim_s, 32,'s'  );  #  2 - # of bits per voxel 1, 8, 16, 32, or 64.
   $h->{dim_un0}       = destruct(\$dim_s, 34,'s'  );  #  2 -
   $h->{pixdim}        =[destruct(\$dim_s, 36,'f8' )]; # 32 - pixdim[] specifies the voxel dimensions:
                                                       #        pixdim[1] - voxel width
                                                       #        pixdim[2] - voxel height
                                                       #        pixdim[3] - interslice distance
                                                       #        ..etc
   $h->{vox_offset}    = destruct(\$dim_s, 68,'f' );   #  4 - byte offset in .img at which voxels start.
                                                       #        This value can be negative to specify 
                                                       #        that the absolute value is applied for
                                                       #        every image
   $h->{scale_factor}  = destruct(\$dim_s, 72,'f' );   #  4 = funused1; scale factor used by SPM
   $h->{funused1}      = destruct(\$dim_s, 76,'f' );   #  4
   $h->{funused2}      = destruct(\$dim_s, 80,'f' );   #  4
   $h->{cal_max}       = destruct(\$dim_s, 84,'f' );   #  4 - calibrated max & min:
   $h->{cal_min}       = destruct(\$dim_s, 88,'f' );   #  4   www.mailbase.ac.uk/lists/spm/2000-09/0099.html
   $h->{compressed}    = destruct(\$dim_s, 92,'f' );   #  4
   $h->{verified}      = destruct(\$dim_s, 96,'f' );   #  4
   $h->{glmax}         = destruct(\$dim_s,100,'i' );   #  4 - global max | pixel values
   $h->{glmin}         = destruct(\$dim_s,104,'i' );   #  4 - global min | (entire database)
                                                       #108 bytes    
   
   $h->{descrip}       = destruct(\$his_s,  0,'a80');  # 80
   $h->{aux_file}      = destruct(\$his_s, 80,'a24');  # 24
   $h->{orient}        = destruct(\$his_s,104,'c'  );  #  1 - slice orientation           | disregarded
                                                       #        0 - transverse unflipped  | by SPM
                                                       #        1 - coronal    unflipped  | instead a
                                                       #        2 - sagittal   unflipped  | .mat file is
                                                       #        3 - transverse flipped    | written out
                                                       #        4 - coronal    flipped    | with this
                                                       #        5 - sagittal   flipped    | information
   $h->{originator}    =[destruct(\$his_s,105,'s5' )]; # 10 - origin
                                                       #        originator[0] x-origin    | non standard
                                                       #        originator[1] y-origin    | SPM use only
                                                       #        originator[2] z-origin    |
   $h->{generated}     = destruct(\$his_s,115,'a10');  # 10
   $h->{scannum}       = destruct(\$his_s,125,'a10');  # 10
   $h->{patient_id}    = destruct(\$his_s,135,'a10');  # 10
   $h->{exp_date}      = destruct(\$his_s,145,'a10');  # 10
   $h->{exp_time}      = destruct(\$his_s,155,'a10');  # 10
   $h->{hist_un0}      = destruct(\$his_s,165,'a3' );  #  3
   $h->{views}         = destruct(\$his_s,168,'i'  );  #  4
   $h->{vols_added}    = destruct(\$his_s,172,'i'  );  #  4
   $h->{start_field}   = destruct(\$his_s,176,'i'  );  #  4
   $h->{field_skip}    = destruct(\$his_s,180,'i'  );  #  4
   $h->{omax}          = destruct(\$his_s,184,'i'  );  #  4
   $h->{omin}          = destruct(\$his_s,188,'i'  );  #  4
   $h->{smax}          = destruct(\$his_s,192,'i'  );  #  4
   $h->{smin}          = destruct(\$his_s,196,'i'  );  #  4                         
                                                       #200 bytes
   return $h;
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
         " smin           <$h->{smin}>\n";  
   }

# Write an ANALYZE 7.5 header to a file
sub write_analyze_header{
   my($h, $hdrfile) = @_;
   
   my $hdr_s = pack("i",  $h->{sizeof_hdr}          );
   $hdr_s .= pack("a10",  $h->{data_type}           );
   $hdr_s .= pack("a18",  $h->{db_name}             );
   $hdr_s .= pack("i",    $h->{extents}             );
   $hdr_s .= pack("s",    $h->{session_error}       );
   $hdr_s .= pack("a",    $h->{regular}             );
   $hdr_s .= pack("c",    $h->{hkey_un0}            );
 
   my $dim_s = pack("s8", @{$h->{dim}}[0..7]        );
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
 
   my $his_s = pack("a80",$h->{descrip}             );
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
   
   open(HDR, ">$hdrfile") or die "Error opening file: $hdrfile\n";
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
   my($h, $mncfile, $imgfile) = @_;
   
   my(%dtypes) = (
                 0 => ['-short', '-unsigned'],
                 2 => ['-byte',  '-unsigned'],
                 4 => ['-short',   '-signed'],
                 8 => ['-long',    '-signed'],
                16 => ['-float'],
                64 => ['-double'],
               128 => ['-byte', '-unsigned', '-vector', '3']
                  );
               
   # Check the data type
   if (!defined($dtypes{$h->{datatype}})){ die "Unknown data type: $h->{datatype}\n"; }

   # Gethistory string
   chomp(my($history) = `date`);
   $history .= '>>>> ' . join(' ', $prog, @ARGV);

   # Get step info
   my(@world_order);
   if (($h->{orient} == 0) || ($h->{orient} == 3)){     # Transverse
      $h->{orientation} = '-transverse';
      @world_order = (1, 2, 3);
      }
   elsif (($h->{orient} == 1) || ($h->{orient} == 4)){  # Coronal
      $h->{orientation} = '-coronal';
      @world_order = (1, 3, 2);
      }
   elsif (($h->{orient} == 2) || ($h->{orient} == 5)){  # Sagittal
      $h->{orientation} = '-sagittal';
      @world_order = (3, 1, 2);
      }
   else{                                                # Unknown
      warn "Unknown data orientation: assuming transverse\n";
      $h->{orientation} = '-transverse';
      @world_order = (1, 2, 3);
      }
   my(@steps) = @{$h->{pixdim}}[@world_order];

   # set up the steps in mm co-ordinates (as opposed to ANALYZE voxel co-ordinates)
   my($c) = 0;
   foreach (@steps){

      print "$c: $_ @{$h->{originator}}[$c]\n";
   
      @{$h->{originator}}[$c] = (@{$h->{originator}}[$c])*$_;
   
      print "$c: $_ @{$h->{originator}}[$c]\n";
      $c ++;
      }

   # Set up rawtominc command
   my($args) = "rawtominc $clobber @{$dtypes{$h->{datatype}}} ".
               "-range $h->{glmin} $h->{glmax} ";

   $args .= ($h->{cal_min} < $h->{cal_max})
               ? "-real_range $h->{cal_min} $h->{cal_max} "
               : "-scan_range ";

   $args .= "$h->{orientation} ".
            "-xstep $steps[0] -ystep $steps[1] -zstep $steps[2] ".
            "-origin @{$h->{originator}}[0, 1, 2] ".
            "-sattribute :history='$history' ".
            "-input $imgfile $mncfile ";

   $args .= (@{$h->{dim}}[4] > 1) ? "@{$h->{dim}}[4] " : '';
   $args .= "@{$h->{dim}}[3, 2, 1]\n";

   # Check for compressed input
   if (!-e $imgfile && -e "$imgfile.gz"){
      system("gunzip $imgfile.gz\n") ==  0 or die "Died during gunzip\n";
      }

   if ($verbose){ print STDOUT $args; }
   system($args) == 0 or die "Died during rawtominc system command\n";
   }

# Read a minc file header and return an appropriate ANALYZE 7.5 header
sub read_minc{
   my($mncfile) = shift;
   my(@start, @step, @length, @dir_cosines, @dims, $permutation);
   
   my($h) = create_analyze_header();
#   ($h->{cal_min}, $h->{cal_max}) = volume_minmax($mncfile);
   
   $h->{glmin} = 0;
   $h->{glmax} = 65535;
   
   volume_params($mncfile, \@start, \@step, \@length, \@dir_cosines, \@dims);
   $permutation = (&get_dimension_order($mncfile))[1];
   
   my $c = 0;
   $h->{dim}[$c] = $h->{pixdim}[$c] = $#{$permutation} + 2;
   foreach (@{$permutation}){
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
                   'long'   => [  8, 32, ],   # signed
                   'float'  => [ 16, 32, ],
                   'double' => [ 64, 64, ],
                   'vector' => [128, 24, ],
                 );
   
   chomp(my $dt   = `mincinfo -vartype image $mncfile`);
   $h->{datatype} = $dtypes{$dt}[0];
   $h->{bitpix}   = $dtypes{$dt}[1];
   
   return $h;
   }

# Takes a 4x4 transformation matrix $M (using the Math:MatrixReal module)
# Returns the translation, scale, shear and rotations encoded in the input
# matrix.
#
# Andrew Janke - a.janke@gmail.com
# Losely based on Louis Collins' make_rots.c from the mni_autoreg package
# With substantial help from Mark Griffin - mark.griffin@cmr.uq.edu.au
sub decompose_transformation_matrix{
   my($M, $Trans, $Scale, $Shear, $Rotn) = @_;

   my ($Sx, $Sy, $Sz, $SHa, $SHb, $SHc, $Rx, $Ry, $Rz);

   # TRANSLATIONS - [M] = [T][S][SH][R]
   # As of yet I am assuming the center of rotation is (0,0,0) as
   # I am not exactly sure as to what SPM does here.
   @$Trans[0] = $M->element(1, 4);
   @$Trans[1] = $M->element(2, 4);
   @$Trans[2] = $M->element(3, 4);
   
   my $T = $M->shadow(); $T->one();         # Create and zero the Translation Matrix
   $T->assign(1, 4, @$Trans[0]);
   $T->assign(2, 4, @$Trans[1]);
   $T->assign(3, 4, @$Trans[2]);
   
   # SCALES - [M] = inv[T][T][S][SH][R] = [S][SH][R]
   # Here we use an identical method to Louis Collins's in mni_autoreg
   # Namely multiply a unit vector in each direction and measure the length
   # after the transformation.
   $M = $T->decompose_LR()->invert_LR() * $M;
   my $SSHRinv = $M->decompose_LR()->invert_LR();
   
   my $Unit = Math::MatrixReal->new(4, 1);
   $Unit->zero(); $Unit->assign(1, 1, 1); $Sx = ($SSHRinv * $Unit)->length();
   $Unit->zero(); $Unit->assign(2, 1, 1); $Sy = ($SSHRinv * $Unit)->length();
   $Unit->zero(); $Unit->assign(3, 1, 1); $Sz = ($SSHRinv * $Unit)->length();
   
   my $Sinv = $M->shadow(); $Sinv->zero();   # Create and zero the inverse Scaling Matrix
   $Sinv->assign(1, 1, $Sx);     $Sx = 1/$Sx;
   $Sinv->assign(2, 2, $Sy);     $Sy = 1/$Sy;
   $Sinv->assign(3, 3, $Sz);     $Sz = 1/$Sz;
  
  
   # SHEARS - [M] = inv[T][T]inv[S][S][SH][R] = [SH][R]
   # We assume the shear matrix:      SH [ 1 0 0 0 ]
   # where x' = x                        [ a 1 0 0 ]
   #       y' = ax + y                   [ b c 1 0 ]
   #       z' = bx + cy + z              [ 0 0 0 1 ]
   # 
   # However as M at this point is in fact [SH][R]
   # we can extract a, b and c as such:
   # 
   # let [ M1 ]
   #     [ M2 ]  =  [SH][R]
   #     [ M3 ] 
   #
   # thus:
   # 
   # a = (M2 . M1) / |M1|^2
   # b = (M3 . M1) / |M1|^2
   # c = (M3 . T)  / |T|^2     where T = M2 - (a . M1)
   #
   # We could also use the determinant to determine whether we have an 
   # Orthogonal matrix and thus don;t have shears, but we don't do this yet....
   
   $M = $Sinv * $M;
   
   # check determinant for "sheariness" if det != 0 shears exist.
   # my $det = $M->decompose_LR()->det_LR();
   
   my $M1 = ~$M->row(1);
   my $M2 = ~$M->row(2);
   my $M3 = ~$M->row(3);
   
   $SHa = $M2->scalar_product($M1) / $M1->scalar_product($M1);
   $SHb = $M3->scalar_product($M1) / $M1->scalar_product($M1);
   my $TMP = $M2 - ($SHa * $M1);
   $SHc = $M3->scalar_product($TMP) / $TMP->scalar_product($TMP);
   
   my $SH = $M->shadow(); $SH->one();      # Create and zero the Shear Matrix
   $SH->assign(2, 1, $SHa);
   $SH->assign(3, 1, $SHb);
   $SH->assign(3, 2, $SHc);
   
   
   # ROTATIONS - [M] = inv[T][T]inv[S][S]inv[SH][SH][R] = [R]
   # We assume cy is positive to ensure we get one of the 2 possible solutions
   # where rotations are between -pi and pi.
   #
   # Here we deduce Rx, Ry and Rz by virtue or the fact that the rotation
   # matrix is as follows. 
   #
   # R = [ cos(Ry)*cos(Rz)  <stuff>          <stuff>          0 ]
   #     [ cos(Ry)*sin(Rz)  <stuff>          <stuff>          0 ]
   #     [ sin(Ry)          sin(Rx)*cos(Ry)  cos(Rx)*cos(Ry)  0 ]
   #     [ 0                0                0                0 ]
   #
   # Then the quadrant of the angle must be deduced by the sign of
   # cos and sin for the particular rotation.
   
   $M = $SH->decompose_LR()->invert_LR() * $M;
   
   # Get Y Rotation and check that we aren't up a creek without a paddle
   my $sy = $M->element(3, 1);
   if (abs($sy) == 1) { die "cos X = 0. I haven't solved this yet...\n"; }
   $Ry = asin($sy);
   
   # Get X Rotation
   my $cy = cos($Ry);
   my $sx = $M->element(3, 2) / $cy;
   my $cx = $M->element(3, 3) / $cy;
   $Rx = asin($sx);
   if ($cx < 0){
      if ($sx > 0){ $Rx =  pi() - $Rx; }   # quadrant 2
      else        { $Rx = -pi() - $Rx; }   # quadrant 3
      }
   
   # Get Z Rotation
   my $cz = $M->element(1, 1) / $cy;
   my $sz = $M->element(2, 1) / $cy;
   $Rz = asin($sz);
   if ($cz < 0){ 
      if ($sz > 0){ $Rz =  pi() - $Rz; }   # quadrant 2
      else        { $Rz = -pi() - $Rz; }   # quadrant 3
      }
   
   
   # If verbose do a bit of checking and output the remainder which should
   # be the identity matrix or close to it  
   if ($verbose){ 
      my $RX = $M->shadow(); $RX->one();     # Create and zero the X Rotation Matrix
      $RX->assign(2, 2,  cos($Rx));   $RX->assign(2, 3, -sin($Rx));
      $RX->assign(3, 2,  sin($Rx));   $RX->assign(3, 3,  cos($Rx));
   
      my $RY = $M->shadow(); $RY->one();     # Create and zero the Y Rotation Matrix
      $RY->assign(1, 1,  cos($Ry));   $RY->assign(1, 3, -sin($Ry));
      $RY->assign(3, 1,  sin($Ry));   $RY->assign(3, 3,  cos($Ry));
   
      my $RZ = $M->shadow(); $RZ->one();     # Create and zero the Z Rotation Matrix
      $RZ->assign(1, 1,  cos($Rz));   $RZ->assign(1, 2, -sin($Rz));
      $RZ->assign(2, 1,  sin($Rz));   $RZ->assign(2, 2,  cos($Rz));
   
      my $RZYXinv = ($RZ * $RY * $RX)->decompose_LR()->invert_LR();
      print "Remainder:\n" . ($RZYXinv * $M);
      }
   
   @$Scale[0] = $Sx;   @$Scale[1] = $Sy;   @$Scale[2] = $Sz;
   @$Shear[0] = $SHa;  @$Shear[1] = $SHb;  @$Shear[2] = $SHc;
   @$Rotn[0]  = $Rx;   @$Rotn[1]  = $Ry;   @$Rotn[2]  = $Rz;
   }

# Creates a 4x4 transformation matrix $M using the input
# Translations, scales, shears and rotations (or not)
sub create_transformation_matrix{
   my($Trans, $Scale, $Shear, $Rotn) = @_;

   # set a few defaults
   my $c;
   for ($c = 0; $c < 3; $c ++){
      if (!defined @$Trans[$c]){ @$Trans[$c] = 0; }
      if (!defined @$Scale[$c]){ @$Scale[$c] = 1; }
      if (!defined @$Shear[$c]){ @$Shear[$c] = 0; }
      if (!defined @$Rotn[$c] ){ @$Rotn[$c]  = 0; }
      }

   my $M = Math::MatrixReal->new(4, 4); $M->one();
   $M->assign(1, 4, @$Trans[0]);
   $M->assign(2, 4, @$Trans[1]); 
   $M->assign(3, 4, @$Trans[2]);
   
   $M->assign(1, 1, @$Scale[0]);
   $M->assign(2, 2, @$Scale[1]);
   $M->assign(3, 3, @$Scale[2]);
  
   my $SH = $M->shadow(); $SH->one();      # Create and zero the Shear Matrix
   $SH->assign(2, 1, @$Shear[0]);
   $SH->assign(3, 1, @$Shear[1]);
   $SH->assign(3, 2, @$Shear[2]);
   
   my $RX = $M->shadow(); $RX->one();     # Create and zero the X Rotation Matrix
   $RX->assign(2, 2,  cos(@$Rotn[0]));   $RX->assign(2, 3, -sin(@$Rotn[0]));
   $RX->assign(3, 2,  sin(@$Rotn[0]));   $RX->assign(3, 3,  cos(@$Rotn[0]));
   
   my $RY = $M->shadow(); $RY->one();     # Create and zero the Y Rotation Matrix
   $RY->assign(1, 1,  cos(@$Rotn[1]));   $RY->assign(1, 3, -sin(@$Rotn[1]));
   $RY->assign(3, 1,  sin(@$Rotn[1]));   $RY->assign(3, 3,  cos(@$Rotn[1]));
   
   my $RZ = $M->shadow(); $RZ->one();     # Create and zero the Z Rotation Matrix
   $RZ->assign(1, 1,  cos(@$Rotn[2]));   $RZ->assign(1, 2, -sin(@$Rotn[2]));
   $RZ->assign(2, 1,  sin(@$Rotn[2]));   $RZ->assign(2, 2,  cos(@$Rotn[2]));
   
   return $M * $SH * $RZ * $RY * $RX;
   }

sub print_transformation_matrix{
   my($M, $name) = @_;
   my (@Trans, @Scale, @Shear, @Rotn, @Rotnd);
   
   print "$name:\n$M";
   decompose_transformation_matrix($M, \@Trans, \@Scale, \@Shear, \@Rotn);
   foreach (@Rotn){ push(@Rotnd, rad2deg($_)); }
   printf("Translation: %10.5f %10.5f %10.5f\n", @Trans);
   printf("Rotation:    %10.5f %10.5f %10.5f\n", @Rotnd);
   printf("Scale:       %10.5f %10.5f %10.5f\n", @Scale);
   printf("Shear:       %10.5f %10.5f %10.5f\n", @Shear);
   }
