# Special version for files at Johns Hopkins
# Dicom files from GE mri, probably via unknown intermediary
# Based on subroutines for GE mri machines, version 6.x dicom

# Routine to initialize tape drive
sub dicomfile_initialize_tape_drive {
   &cleanup_and_die("dicomfile_to_minc does not support tape reading.\n",1);
}

# Routine to get a string from the header
sub acr_find_string { 
   if (scalar(@_) != 3) {
      die "Argument error in numaris3_find_string";
   }
   local(*header, $group, $element) = @_;

   local($grstr) = sprintf("0x%04x", $group);
   local($elstr) = sprintf("0x%04x", $element);

   return $header{$grstr, $elstr, 'string'};
}

# Routine to get an array of values from the header
sub acr_find_numeric {
   local(@values) = split(/\\/, &acr_find_string(@_));
   foreach $value (@values) {
      $value += 0;
   }
   return (scalar(@values) > 1) ? @values : $values[0];
}

# Routine to get an integer from the header
sub acr_find_int { 
   if (scalar(@_) != 3) {
      die "Argument error in numaris3_find_int";
   }
   local(*header, $group, $element) = @_;

   local($grstr) = sprintf("0x%04x", $group);
   local($elstr) = sprintf("0x%04x", $element);

   return $header{$grstr, $elstr, 'short'};
}

# Routine to convert world coordinates
sub convert_coordinates {
    local(@coords) = @_;
    $coords[0] *= -1;
    $coords[1] *= -1;
    return @coords;
}

# Routine to compute a dot product
sub vector_dot_product {
    local(*vec1, *vec2) = @_;
    local($result, $i);
    $result = 0;
    for $i (0..2) {
        $result += $vec1[$i] * $vec2[$i];
    }
    return $result;
}

# Routine to compute a vector cross product
sub vector_cross_product {
    local(*vec1, *vec2) = @_;
    local(@result);
    $#result = 2;
    $result[0] = $vec1[1] * $vec2[2] - $vec1[2] * $vec2[1];
    $result[1] = $vec1[2] * $vec2[0] - $vec1[0] * $vec2[2];
    $result[2] = $vec1[0] * $vec2[1] - $vec1[1] * $vec2[0];
    return @result;
}

# Subroutine to read the headers
sub dicomfile_read_headers {

    # Set constants for reading file
    local($header_maxid) = "0x0029";
    local($expected_manufacturer) = "GE MEDICAL SYSTEMS";

    # Check arguements
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in dicomfile_read_headers",1);
    }
    local($filename, *header) = @_;

    # Dump the header
    local($group, $element, $data);
    open(DUMP, "extract 132 -1 $filename | dump_acr_nema - $header_maxid|");
    while (<DUMP>) {
       chop;
       if (/^\s*(0x[\da-f]{4,4})\s+(0x[\da-f]{4,4})\s+length = \d+ :(.*)$/) {
          $group = $1;
          $element = $2;
          $data = $3;
          if ($data =~ /string = "(.*)"$/) {
             $header{$group, $element, 'string'} = $1;
          }
          if ($data =~ /short = (\d+)/) {
             $header{$group, $element, 'short'} = $1;
          }
       }
    }
    close(DUMP);

    # Check the return status
    if ($? != 0) {
       warn "Error dumping header for file $filename";
       return 1;
    }

    # Check for magic in header
    local($manufacturer) = &acr_find_string(*header, 0x8, 0x70);
    if ($manufacturer ne $expected_manufacturer) {
       warn "Unrecognized manufacturer \"$manufacturer\" - " .
          "expected \"$expected_manufacturer\"\n";
       return 1;
    }

    return 0;
}

# Routine to get Numaris 3 file info
sub dicomfile_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local(%header);
    undef(%header);
    if (&dicomfile_read_headers($filename, *header)) {
        return 1;
    }

    # Get interesting values
    $file_info{'numechos'} = &acr_find_numeric(*header, 0x21, 0x107e);
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    ($file_info{'exam'} = &acr_find_string(*header, 0x20, 0x10))
       =~ s/[^\w-\.]//g;
    $file_info{'series'} = &acr_find_numeric(*header, 0x20, 0x11);
    $file_info{'image'} = &acr_find_numeric(*header, 0x20, 0x13);

    $file_info{'echo'} = &acr_find_numeric(*header, 0x18, 0x86);
    $file_info{'width'} = &acr_find_int(*header, 0x28, 0x11);
    $file_info{'height'} = &acr_find_int(*header, 0x28, 0x10);
    local($bits_alloc) = &acr_find_int(*header, 0x28, 0x100);
    if ($bits_alloc != 16) {
       warn "Wrong number of bits allocated per image ($bits_alloc)\n";
       return 1;
    }
    $file_info{'pixel_size'} = 2;
    $file_info{'patient_name'} = &acr_find_string(*header, 0x10, 0x10);

    # Get slice position and orientation (row and column vectors)
    local(@position) = 
       &convert_coordinates(&acr_find_numeric(*header, 0x20, 0x32));
    if (scalar(@position) != 3) {
       warn "************** Error reading slice position ***************\n";
    }
    local(@array) = &acr_find_numeric(*header, 0x20, 0x37);
    if (scalar(@array) != 6) {
       warn "************* Error reading slice orientation *************\n";
    }
    local(@column) = &convert_coordinates(@array[0..2]);
    local(@row) = &convert_coordinates(@array[3..5]);

    # Figure out normal and orientation
    local(@normal) = 
       &vector_cross_product(*column, *row);
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
       &acr_find_numeric(*header, 0x28, 0x30);
    if (length($file_info{'rowstep'}) <= 0) {
       $file_info{'colstep'} = $file_info{'rowstep'} = 1;
    }
    $file_info{'colstep'} *= -1.0;
    $file_info{'rowstep'} *= -1.0;
    @col_dircos = &get_dircos(@column);
    @row_dircos = &get_dircos(@row);
    @slc_dircos = &get_dircos(@normal);
    $file_info{'slicepos'} = &vector_dot_product(*position, *slc_dircos);
    $file_info{'colstart'} = &vector_dot_product(*position, *col_dircos)
       + $file_info{'colstep'} / 2;
    $file_info{'rowstart'} = &vector_dot_product(*position, *row_dircos)
       + $file_info{'rowstep'} / 2;
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
    $file_info{'tr'} = &acr_find_numeric(*header, 0x18, 0x80)/1000;
    $file_info{'te'} = &acr_find_numeric(*header, 0x18, 0x81)/1000;
    $file_info{'ti'} = &acr_find_numeric(*header, 0x18, 0x82)/1000;
    $file_info{'mr_flip'} = &acr_find_numeric(*header, 0x18, 0x1314);
    ($file_info{'patient_age'} = &acr_find_string(*header, 0x10, 0x1010))
        =~ s/\D//g;
    local($sex_flag) = &acr_find_string(*header, 0x10, 0x40);
    if ($sex_flag eq 'M ') {
        $file_info{'patient_sex'} = "male__";
    }
    elsif ($sex_flag eq 'F ') {
        $file_info{'patient_sex'} = "female";
    }
    $file_info{'patient_id'} = &acr_find_string(*header, 0x10, 0x20);
    $file_info{'institution'} = &acr_find_string(*header, 0x8, 0x80);
    local($study_date, $study_time);
    ($study_date = &acr_find_string(*header, 0x8, 0x22)) =~ s/\./-/g;
    $study_time = &acr_find_string(*header, 0x8, 0x32);
    $file_info{'start_time'} = "$study_date $study_time";

    # Get specific file info
    local($compression_code) = &acr_find_string(*header, 0x28, 0x60);
    if (($compression_code ne "NONE") && ($compression_code ne "")) {
       warn "File is compressed\n";
       return 1;
    }
    $specific_file_info{'pixel_data_group'} = "0x7fe0";
    $specific_file_info{'pixel_data_element'} = "0x10";

    return 0;
}

# Routine to get return command that will extract image
sub dicomfile_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in dicomfile_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract 132 -1 $cur_file | " .
        "extract_acr_nema " . 
        $specific_file_info{'pixel_data_group'} . " " .
        $specific_file_info{'pixel_data_element'} . " " .
        " | byte_swap ";

    return $cmd;
}

# MAIN PROGRAM

*initialize_tape_drive = *dicomfile_initialize_tape_drive;
*read_file_info = *dicomfile_read_file_info;
*get_image_cmd = *dicomfile_get_image_cmd;

&mri_to_minc(@ARGV);

