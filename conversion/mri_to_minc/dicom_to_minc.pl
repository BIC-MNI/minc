# Code for reading generic dicom files for conversion to minc.
# Does not make use of any shadow groups.
# Should read both a stream of dicom groups, or a proper dicom file with
# preamble.

########################################################################

# DICOM 3 CONSTANTS

# Dicom file offset
sub dc3_file_offset {return 128;}

# Length of magic and length field
sub dc3_preamble_length {return 16;}

# Dicom magic
sub dc3_magic_string {return "DICM";}

# Offset to header length value
sub dc3_offset_to_length {return 12;}

# Maximum group number needed for header info
sub dc3_header_maxid {return "0x0029";}

# Exam is unique id of session in scanner (study)
sub dc3_exam {return (0x20, 0x10);}

# Series is id of scan within a session (acquisition)
sub dc3_series {return (0x20, 0x11);}
sub dc3_acquisition {return (0x20, 0x12);}

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

# Image position
sub dc3_image_position {return (0x20, 0x32);}

# Image orientation
sub dc3_image_orientation {return (0x20, 0x37);}

# Repetition, echo and inversion times in ms, plus flip angle in degrees
sub dc3_tr {return (0x18, 0x80);}
sub dc3_te {return (0x18, 0x81);}
sub dc3_ti {return (0x18, 0x82);}
sub dc3_flip {return (0x19, 0x0);}

# Patient birthdate, age, sex, id, institution
sub dc3_patient_birthdate {return (0x10, 0x30);}
sub dc3_patient_age {return (0x10, 0x1010);}
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

########################################################################

# DICOM 3 ROUTINES

# Get a list of all dicom element numbers
sub acr_get_element_numbers {
   if (scalar(@_) != 1) {
      die "Argument error in acr_get_element_numbers";
   }
   local(*header) = @_;

   local(@keys) = grep(/string$/, keys(%header));
   local(@elements) = ();
   local($key);
   foreach $key (@keys) {
      local(@fields) = split($;, $key);
      local($newkey) = $fields[0] . $; . $fields[1];
      push(@elements, $newkey)
   }
   @elements = sort(@elements);

   return @elements;
}

# Routine to get a string from the header given group and element as strings
sub acr_find_string_with_string {
   if (scalar(@_) != 3) {
      die "Argument error in acr_find_string_with_string";
   }
   local(*header, $grstr, $elstr) = @_;

   return $header{$grstr, $elstr, 'string'};
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

    # Figure out how much preamble to skip by looking for the DICOM magic
    # and reading the first element (length) value
    local($file_offset) = 0;
    if (!open(FILE, $filename)) {
       warn "Unable to open file \"$filename\"\n";
       return 1;
    }
    local($buffer) = '';
    if (!seek(FILE, &dc3_file_offset, 0) ||
        (read(FILE, $buffer, &dc3_preamble_length) != &dc3_preamble_length)) {
       warn "Error checking file type on file \"$filename\"\n";
       return 1;
    }

    # Look for the magic string
    if (substr($buffer, 0, length(&dc3_magic_string)) eq &dc3_magic_string) {
       local($value) = substr($buffer, &dc3_offset_to_length, 4);
       $file_offset = unpack("L", $value);
       if ($file_offset > 65535) {
          $file_offset = unpack("L", reverse($value));
       }

       # Read all of the meta information to make sure that the offset
       # is right
       local($offset) = &dc3_file_offset + length(&dc3_magic_string);
       local($length) = ($file_offset + 12) * 2;
       $value = `extract $offset $length $filename | extract_acr_nema -i 2 0`;
       if (length($value) == 4) {
          $file_offset = unpack("L", $value);
          if ($file_offset > 65535) {
             $file_offset = unpack("L", reverse($value));
          }
       }

       # Add the other offsets
       $file_offset += &dc3_file_offset + &dc3_preamble_length;
    }

    # Save the file offset
    $header{'file_offset'} = $file_offset;

    # Dump the header
    local($group, $element, $data);
    open(DUMP, "extract $file_offset -1 $filename | " .
         "dump_acr_nema -i - $header_maxid|");
    while (<DUMP>) {
       chop;
       if (/^\s*(0x[\da-f]{4,4})\s+(0x[\da-f]{4,4})\s+length = \d+ :(.*)$/) {
          $group = $1;
          $element = $2;
          $data = $3;
          if ($data =~ /(string|value) = "(.*)"$/) {
             $header{$group, $element, 'string'} = $2;
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
    $file_info{'numechos'} = 1;
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    $file_info{'exam'} = &acr_find_string(*header, &dc3_exam);
    if (length($file_info{'exam'}) == 0) {
       $file_info{'exam'} = &acr_find_string(*header, &dc3_study_date);
    }
    $file_info{'exam'} =~ s/\W//g;
    local($series) = &acr_find_numeric(*header, &dc3_series);
    local($acquisition) = &acr_find_numeric(*header, &dc3_acquisition);
    local($the_series);
    if (($series > 0) && ($acquisition > 0)) {
       $the_series = $series*1000+$acquisition;
    }
    elsif ($series > 0) {$the_series = $series;}
    else {$the_series = $acquisition;}
    $file_info{'series'} = $the_series;
    local($the_image) = &acr_find_numeric(*header, &dc3_image);
    if (!defined($the_image) || (length($the_image) == 0)) {
       if (!defined($Image_Counter)) {        # Global variable
          $Image_Counter = 1;
       }
       $the_image = $Image_Counter++;
    }
    $file_info{'image'} = $the_image;

    $file_info{'echo'} = &acr_find_numeric(*header, &dc3_echo);
    $file_info{'width'} = &acr_find_int(*header, &dc3_width);
    $file_info{'height'} = &acr_find_int(*header, &dc3_height);
    local($bits_alloc) = &acr_find_int(*header, &dc3_bits_alloc);
    if ($bits_alloc == 16) {
       $file_info{'pixel_size'} = 2;
    }
    elsif ($bits_alloc == 8) {
       $file_info{'pixel_size'} = 1;
    }
    else {
       warn "Wrong number of bits allocated per image ($bits_alloc)\n";
       return 1;
    }
    $file_info{'patient_name'} = &acr_find_string(*header, &dc3_patient_name);

    # Get slice position and orientation (row and column vectors)
    local(@position) = 
       &convert_coordinates(&acr_find_numeric(*header, &dc3_image_position));
    if (scalar(@position) != 3) {
       warn "************** Error reading slice position ***************\n";
    }
    local(@array) = &acr_find_numeric(*header, &dc3_image_orientation);
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
       &acr_find_numeric(*header, &dc3_pixel_size);
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
    $file_info{'tr'} = &acr_find_numeric(*header, &dc3_tr)/1000;
    $file_info{'te'} = &acr_find_numeric(*header, &dc3_te)/1000;
    $file_info{'ti'} = &acr_find_numeric(*header, &dc3_ti)/1000;
    $file_info{'mr_flip'} = &acr_find_numeric(*header, &dc3_flip);
    ($file_info{'patient_birthdate'} = 
       &acr_find_string(*header, &dc3_patient_birthdate)) =~ s/\./-/g;
    ($file_info{'patient_age'} = &acr_find_string(*header, &dc3_patient_age))
        =~ s/\D//g;
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

    # Get dicom element info - only even numbered groups
    local(@elements) = &acr_get_element_numbers(*header);
    local($element);
    foreach $element (@elements) {
       local($group,$element) = split($;, $element);
       if (hex($group) % 2 != 0) {next;}
       local($value) = &acr_find_string_with_string(*header, $group, $element);
       if (defined($value) && length($value) > 0) {
          $file_info{"dicom_$group:el_$element"} = $value;
       }
    }

    # Get specific file info
    local($compression_code) = &acr_find_string(*header, 0x28, 0x60);
    if (($compression_code ne "NONE") && ($compression_code ne "")) {
       warn "File is compressed\n";
       return 1;
    }
    ($specific_file_info{'pixel_data_group'}, 
     $specific_file_info{'pixel_data_element'}) = &dc3_pixel_data;
    $specific_file_info{'file_offset'} = $header{'file_offset'};
    $specific_file_info{'pixel_size'} = $file_info{'pixel_size'};

    return 0;
}

sub dicom3_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in dicom3_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " .
        $specific_file_info{'file_offset'} . 
        " -1 $cur_file" .
        " | extract_acr_nema -i " . 
        $specific_file_info{'pixel_data_group'} . " " .
        $specific_file_info{'pixel_data_element'} . " ";
    if (!$Image_is_big_endian && $specific_file_info{'pixel_size'} > 1) {
       $cmd .= " | byte_swap ";
    }

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

# Check for dicom-specific arguments
$Image_is_big_endian = 0;
@args_to_remove = ();
foreach $i (0..$#ARGV) {
   $_ = $ARGV[$i];
   if (/^-h(elp)?$/) {
      warn 
"Dicom-specific options:
 -big_endian_image:\tSpecify that the image is big endian
";
   }
   elsif (/^-big/ && (index("-big_endian_image",$_)==0)) {
      $Image_is_big_endian = 1;
      push(@args_to_remove, $i);
   }
}
foreach $i (reverse(@args_to_remove)) {
   splice(@ARGV, $i, 1);
}

&mri_to_minc(@ARGV);

