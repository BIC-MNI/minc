# DICOM 3 CONSTANTS

# Maximum group number needed for header info
sub dc3_header_maxid {return "0x0029";}

# Number of echos
sub dc3_numechos {return (0x21, 0x0);}

# Exam is unique id of session in scanner (study)
sub dc3_exam {return (0x20, 0x10);}

# Series is id of scan within a session (acquisition)
sub dc3_series {return (0x20, 0x12);}

# Image is image number
sub dc3_image {return (0x20, 0x13);}

# Echo number
sub dc3_echo {return (0x18, 0x86);}

# Width of image
sub dc3_width {return (0x28, 0x11);}

# Height of image
sub dc3_height {return (0x28, 0x10);}

# Bits allocated
sub dc3_bits_alloc {return (0x28, 0x100);}

# Patient name
sub dc3_patient_name {return (0x10, 0x10);}

# Pixel size
sub dc3_pixel_size {return (0x28, 0x30);}

# Image normal vector
sub dc3_normal {return (0x21, 0x0);}

# Column and row direction cosines (unit vectors)
sub dc3_column_dircos {return (0x21, 0x0);}
sub dc3_row_dircos {return (0x21, 0x0);}

# Slice centre
sub dc3_slice_centre {return (0x21, 0x0);}

# Repetition, echo and inversion times in ms, plus flip angle in degrees
sub dc3_tr {return (0x18, 0x80);}
sub dc3_te {return (0x18, 0x81);}
sub dc3_ti {return (0x18, 0x82);}
sub dc3_flip {return (0x19, 0x0);}

# Patient birthdate, sex, id, institution
sub dc3_patient_birthdate {return (0x10, 0x30);}
sub dc3_patient_sex {return (0x10, 0x40);}
sub dc3_patient_id {return (0x10, 0x20);}
sub dc3_institution {return (0x8, 0x80);}

# Study date and time
sub dc3_study_date {return (0x8, 0x22);}
sub dc3_study_time {return (0x8, 0x32);}

# Compression code
sub dc3_compression {return (0x28, 0x60);}

# Pixel data location
sub dc3_pixel_data {return ("0x7fe0", "0x10");}

# DICOM 3 ROUTINES

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
   return @values;
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

# Subroutine to read the Dicom 3 file headers
sub dicom3_read_headers {

    # Set constants for reading file
    local($header_maxid) = &dc3_header_maxid;

    # Check arguements
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in dicom3_read_headers",1);
    }
    local($filename, *header) = @_;

    # Check that the file exists and is readable
    if (! -r $filename) {
       warn "Unable to open file \"$filename\"";
       return 1;
    }

    # Dump the header
    local($group, $element, $data);
    open(DUMP, "dump_acr_nema $filename $header_maxid|");
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

    return 0;
}

# Routine to get Dicom 3 file info
sub dicom3_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local(%header);
    undef(%header);
    if (&dicom3_read_headers($filename, *header)) {
        return 1;
    }

    # Get interesting values
    $file_info{'numechos'} = &acr_find_numeric(*header, &dc3_numechos);
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    # We cheat and use study date for exam, getting rid of weird characters
    ($file_info{'exam'} = &acr_find_string(*header, &dc3_exam))
       =~ s/\W//g;
    $file_info{'series'} = &acr_find_numeric(*header, &dc3_series);
    $file_info{'image'} = &acr_find_numeric(*header, &dc3_image);

    $file_info{'echo'} = &acr_find_numeric(*header, &dc3_echo);
    $file_info{'width'} = &acr_find_int(*header, &dc3_width);
    $file_info{'height'} = &acr_find_int(*header, &dc3_height);
    local($bits_alloc) = &acr_find_int(*header, &dc3_bits_alloc);
    if ($bits_alloc != 16) {
       warn "Wrong number of bits allocated per image ($bits_alloc)\n";
       return 1;
    }
    $file_info{'pixel_size'} = 2;
    $file_info{'patient_name'} = &acr_find_string(*header, &dc3_patient_name);

    # Get orientation info
    local(@normal, @col_dircos, @row_dircos, @slc_dircos, @dircos);
    @normal = &acr_find_numeric(*header, &dc3_normal);
    if (scalar(@normal) != 3) {
       @normal = (0,0,1);
    }
    local($norm_r) = &abs($normal[0]);
    local($norm_a) = &abs($normal[1]);
    local($norm_s) = &abs($normal[2]);
    local($plane) = 'transverse';
    local($max) = $norm_s;
    @col_dircos = (1,0,0);
    @row_dircos = (0,1,0);
    if ($norm_r > $max) {
        $plane = 'sagittal';
        $max = $norm_r;
        @col_dircos = (0,1,0);
        @row_dircos = (0,0,1);
    }
    if ($norm_a > $max) {
        $plane = 'coronal';
        $max = $norm_a;
        @col_dircos = (1,0,0);
        @row_dircos = (0,0,1);
    }
    $file_info{'orientation'} = $plane;

    # Get coordinate information
    ($file_info{'rowstep'}, $file_info{'colstep'}) = 
       &acr_find_numeric(*header, &dc3_pixel_size);
    if (length($file_info{'rowstep'}) <= 0) {
       $file_info{'colstep'} = $file_info{'rowstep'} = 1;
    }
    $file_info{'colstep'} *= -1.0;
    $file_info{'rowstep'} *= -1.0;
    @dircos = &acr_find_numeric(*header, &dc3_column_dircos);
    if (scalar(@dircos) == 3) {@col_dircos = @dircos;}
    @dircos = &acr_find_numeric(*header, &dc3_row_dircos);
    if (scalar(@dircos) == 3) {@row_dircos = @dircos;}
    @col_dircos = &get_dircos(@col_dircos);
    @row_dircos = &get_dircos(@row_dircos);
    @slc_dircos = &get_dircos(@normal);
    local($xcentre, $ycentre, $zcentre) = 
       &acr_find_numeric(*header, &dc3_slice_centre);
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
    $file_info{'tr'} = &acr_find_numeric(*header, &dc3_tr)/1000;
    $file_info{'te'} = &acr_find_numeric(*header, &dc3_te)/1000;
    $file_info{'ti'} = &acr_find_numeric(*header, &dc3_ti)/1000;
    $file_info{'mr_flip'} = &acr_find_numeric(*header, &dc3_flip);
    ($file_info{'patient_birthdate'} = 
       &acr_find_string(*header, &dc3_patient_birthdate)) =~ s/\./-/g;
    local($sex_flag) = &acr_find_string(*header, &dc3_patient_sex);
    if ($sex_flag eq 'M ') {
        $file_info{'patient_sex'} = "male__";
    }
    elsif ($sex_flag eq 'F ') {
        $file_info{'patient_sex'} = "female";
    }
    $file_info{'patient_id'} = &acr_find_string(*header, &dc3_patient_id);
     $file_info{'institution'} = &acr_find_string(*header, &dc3_institution);
    local($study_date, $study_time);
    ($study_date = &acr_find_string(*header, &dc3_study_date)) =~ s/\./-/g;
    $study_time = &acr_find_string(*header, &dc3_study_time);
    $file_info{'start_time'} = "$study_date $study_time";

    # Get specific file info
    local($compression_code) = &acr_find_string(*header, 0x28, 0x60);
    if (($compression_code ne "NONE") && ($compression_code ne "")) {
       warn "File is compressed\n";
       return 1;
    }
    ($specific_file_info{'pixel_data_group'}, 
     $specific_file_info{'pixel_data_element'}) = &dc3_pixel_data;

    return 0;
}

sub dicom3_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in dicom3_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract_acr_nema " . 
        $cur_file . " " .
        $specific_file_info{'pixel_data_group'} . " " .
        $specific_file_info{'pixel_data_element'} . " " .
        " | byte_swap ";

    return $cmd;
}

# Routine to initialize tape drive
sub dicom3_initialize_tape_drive {
   &cleanup_and_die("dicom_to_minc does not support tape reading.\n",1);
}


# MAIN PROGRAM

*initialize_tape_drive = *dicom3_initialize_tape_drive;
*read_file_info = *dicom3_read_file_info;
*get_image_cmd = *dicom3_get_image_cmd;

&mri_to_minc(@ARGV);

