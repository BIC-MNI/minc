# Subroutines for Siemens Magnetom Vision mri machines, internal format.
#
# Some things to note: I couldn't find any fields giving the image 
# dimensions, so I assume 256x256. Nor could I find anything about offset 
# to image data, so I assume that it runs to the end of the file (the header
# usually is fixed length, but not always).

# Routine to convert Siemens coordinates
sub convert_coordinates {
    local(@coords) = @_;
    $coords[0] *= -1;
    $coords[2] *= -1;
    return @coords;
}

# Routine to initialize tape drive
sub smv_initialize_tape_drive {
   &cleanup_and_die("Tape reading is not supported.\n",1);
}

sub smv_read_header {
   if (scalar(@_) != 2) {
      &cleanup_and_die("Argument error in smv_read_header",1);
   }
   local($filename, *header) = @_;

   # Constants
   local($header_length) = 6144;

   # Open file
   if (!open(SMV, "<".$filename)) {
      warn "Can't open file $filename: $!\n";
      return 1;
   }

   # Read in the header
   if (read(SMV, $header, $header_length) != $header_length) {
      return 1;
   }

   # Close the file
   close(SMV);

   return 0;
}

sub smv_read_file_info {
   if (scalar(@_) != 3) {
      &cleanup_and_die("Argument error in read_file_info",1);
   }
   local($filename, *file_info, *specific_file_info) = @_;

   # Constant
   local($imageoffset) = 6144;

   # Get header
   local($header);
   if (&smv_read_header($filename, *header)) {
      return 1;
   }

   # Get image size
   local($pixelsize) = 2;
   local($imagewidth) = 
      int(sqrt(((-s $filename) - $imageoffset) / $pixelsize));
   local($imagesize) = $imagewidth * $imagewidth * $pixelsize;

   # Get interesting values
   $file_info{'numechos'} = 100;
   # We cheat and use study date for exam, getting rid of weird characters
   $file_info{'exam'} = sprintf("%04d%02d%02d_%02d%02d%02d",
                                &unpack_value(*header, 12, 'I3'),
                                &unpack_value(*header, 52, 'I3'));
   $file_info{'series'} = &unpack_value(*header, 5943, 'A2')+0;
   $file_info{'image'} = &unpack_value(*header, 5546, 'A4')+0;

   $file_info{'echo'} = &unpack_value(*header, 5752, 'A1')+0;
   $file_info{'pixel_size'} = $pixelsize;
   $file_info{'width'} = $imagewidth;
   $file_info{'height'} = $file_info{'width'};
   $file_info{'patient_name'} = &unpack_value(*header, 768, 'A25');
   local(@normal) = 
      &convert_coordinates(&unpack_value(*header, 3792, 'd3'));
   if (scalar(@normal) != 3) {
      @normal = (0,0,1);
   }
   local($norm_r) = &abs($normal[0]);
   local($norm_a) = &abs($normal[1]);
   local($norm_s) = &abs($normal[2]);
   local($plane) = 'transverse';
   local($max) = $norm_s;
   if ($norm_r > $max) {
      $plane = 'sagittal';
      $max = $norm_r;
   }
   if ($norm_a > $max) {
      $plane = 'coronal';
      $max = $norm_a;
   }
   $file_info{'orientation'} = $plane;

    # Get coordinate information
   local(@col_dircos, @row_dircos, @slc_dircos);
   ($file_info{'rowstep'}, $file_info{'colstep'}) = 
      &unpack_value(*header, 5000, 'd2');
    if (length($file_info{'rowstep'}) <= 0) {
       $file_info{'colstep'} = $file_info{'rowstep'} = 1;
    }
    $file_info{'colstep'} *= -1.0;
    $file_info{'rowstep'} *= -1.0;
    @col_dircos = &get_dircos
       (&convert_coordinates(&unpack_value(*header, 3832, 'd3')));
   @row_dircos = &get_dircos
      (&convert_coordinates(&unpack_value(*header, 3856, 'd3')));
   @slc_dircos = &get_dircos(@normal);
   local($xcentre, $ycentre, $zcentre) = 
      &convert_coordinates(&unpack_value(*header, 3768, 'd3'));
   $file_info{'slicepos'} = $xcentre * $slc_dircos[0] + 
      $ycentre * $slc_dircos[1] + $zcentre * $slc_dircos[2];
   $file_info{'colstart'} =
      ($xcentre * $col_dircos[0] + 
       $ycentre * $col_dircos[1] + 
       $zcentre * $col_dircos[2]) - 
          $file_info{'colstep'} * ($file_info{'width'} - 1) / 2;
   $file_info{'rowstart'} =
      ($xcentre * $row_dircos[0] + 
       $ycentre * $row_dircos[1] + 
       $zcentre * $row_dircos[2]) - 
          $file_info{'rowstep'} * ($file_info{'height'} - 1) / 2;
   $file_info{'col_dircos_x'} = $col_dircos[0];
   $file_info{'col_dircos_y'} = $col_dircos[1];
   $file_info{'col_dircos_z'} = $col_dircos[2];
   $file_info{'row_dircos_x'} = $row_dircos[0];
   $file_info{'row_dircos_y'} = $row_dircos[1];
   $file_info{'row_dircos_z'} = $row_dircos[2];
   $file_info{'slc_dircos_x'} = $slc_dircos[0];
   $file_info{'slc_dircos_y'} = $slc_dircos[1];
   $file_info{'slc_dircos_z'} = $slc_dircos[2];

   # Get other info
   $file_info{'tr'} = &unpack_value(*header, 5734, 'A7') / 1000;
   $file_info{'te'} = &unpack_value(*header, 5746, 'A5') / 1000;
   $file_info{'ti'} = undef;
   $file_info{'mr_flip'} = &unpack_value(*header, 5714, 'A3');
   $file_info{'patient_birthdate'} = 
      sprintf("%04d-%02d-%02d", &unpack_value(*header, 808, 'I3'));
   local($sex_flag) = &unpack_value(*header, 5517, 'A1');
   if ($sex_flag eq 'M ') {
      $file_info{'patient_sex'} = "male__";
   }
   elsif ($sex_flag eq 'F ') {
      $file_info{'patient_sex'} = "female";
   }
   $file_info{'patient_id'} = &unpack_value(*header, 795, 'A12');
   $file_info{'institution'} = &unpack_value(*header, 105, 'A25');
   $file_info{'start_time'} = sprintf("%04d-%02d-%02d %02d:%02d:%02d",
                                      &unpack_value(*header, 12, 'I3'),
                                      &unpack_value(*header, 52, 'I3'));

    # Get specific file info
   $specific_file_info{'pixel_data_offset'} = $imageoffset;
   $specific_file_info{'pixel_data_len'} = 
      $file_info{'width'} * $file_info{'height'} * $file_info{'pixel_size'};

   return 0;

}

sub smv_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in smv_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " . 
        $specific_file_info{'pixel_data_offset'} . " " .
        $specific_file_info{'pixel_data_len'} . " " .
        $cur_file;

    return $cmd;
}

*initialize_tape_drive = *smv_initialize_tape_drive;
*read_file_info = *smv_read_file_info;
*get_image_cmd = *smv_get_image_cmd;

&mri_to_minc(@ARGV);
