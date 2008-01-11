#! /usr/bin/env perl 
#
# Andrew Janke - a.janke@gmail.com
# Center for Magnetic Resonance
# The University of Queensland
#
# ANALYZE is a commercial file format from the mayo clinic: www.mayo.edu/bir
# This script merely dumps the contents of a header file
#
# Tue Feb  6 09:49:25 EST 2001 - created

use warnings "all";
use strict;

chomp(my($prog) = `basename $0`);
my($Usage) = "Usage: $prog <ana.hdr>\n";

# Check arguments
if ($#ARGV < 0){ die $Usage; }

my $anafile = $ARGV[0];
$anafile =~ s/\....$//;
   
if (!-e "$anafile.hdr"){ die "$prog: Couldn't find $anafile.hdr\n"; }

print STDOUT dump_analyze_header(read_analyze_header("$anafile.hdr"));


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
