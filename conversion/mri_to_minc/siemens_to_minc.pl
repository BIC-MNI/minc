# Subroutines for Siemens mri machines

# Routine to take absolute value
sub abs {
    local(@new, $val);
    foreach $val (@_) {
        push(@new, ($val<=>0) * $val);
    }
    return @new;
}

# Routine to get a VAX integer
sub unpack_int {
    if (scalar(@_) != 2) {
        die "Argument error in unpack_int\n";
    }
    local(*string, $offset) = @_;

    return unpack("i", scalar(reverse(substr($string, $offset, 4))));
}

# Routine to get a VAX short
sub unpack_short {
    if (scalar(@_) != 2) {
        die "Argument error in unpack_short\n";
    }
    local(*string, $offset) = @_;

    return unpack("s", scalar(reverse(substr($string, $offset, 2))));
}

# Routine to get a VAX integer
sub unpack_float {
    if (scalar(@_) != 2) {
        die "Argument error in unpack_float\n";
    }
    local(*string, $offset) = @_;

    return unpack("f", 
                  scalar(reverse(substr($string, $offset, 2))).
                  scalar(reverse(substr($string, $offset+2, 2)))) / 4.0;
}

# Routines to do vector and matrix operations

sub vector_normalize {
    local(*vector) = @_;
    local(@result);
    $#result = 2;
    local($scale);
    local($mag) = 0;
    for $i (0..2) {
        $mag += ($vector[$i] ** 2);
    }
    $mag = sqrt($mag);
    if ($mag > 0) {$scale = 1 / $mag;}
    else {$scale = 1;}
    for $i (0..2) {
        $result[$i] = $scale * $vector[$i];
    }
    return @result;
}

sub vector_equal {
    local(*vec1, *vec2) = @_;
    local($result) =1;
    for $i (0..2) {
        $result = ($result && ($vec1[$i] == $vec2[$i]));
    }
    return $result;
}

sub vector_dot_product {
    local(*vec1, *vec2) = @_;
    local(@result);
    $#result = 2;
    for $i (0..2) {
        $result[$i] = $vec1[$i] * $vec2[$i];
    }
    return @result;
}

sub vector_cross_product {
    local(*vec1, *vec2) = @_;
    local(@result);
    $#result = 2;
    $result[0] = $vec1[1] * $vec2[2] - $vec1[2] * $vec2[1];
    $result[1] = $vec1[2] * $vec2[0] - $vec1[0] * $vec2[2];
    $result[2] = $vec1[0] * $vec2[1] - $vec1[1] * $vec2[0];
    return @result;
}

sub matrix_multiply {
    local(*mat1, *mat2) = @_;
    local(@result);
    $#result = 8;
    local($temp);
    for $i (0..2) {
        for $j (0..2) {
            $temp = 0;
            for $k (0..2) {
                $temp += $mat1[$i*3 + $k] * $mat2[$k*3 + $j];
            }
            $result[$i*3+$j] = $temp;
        }
    }
    return @result;
}

sub matrix_vector_multiply {
    local(*matrix, *vector) = @_;
    local(@result);
    $#result = 2;
    local($temp);
    for $i (0..2) {
        $temp = 0;
        for $j (0..2) {
            $temp += $matrix[$i*3 + $j] * $vector[$j];
        }
        $result[$i] = $temp;
    }
    return @result;
}

sub matrix_transpose {
    local(*matrix) = @_;
    local(@result);
    $#result = 8;
    for $i (0..2) {
        for $j (0..2) {
            $result[$i*3 + $j] = $matrix[$j*3 + $i];
        }
    }
    return @result;
}

sub matrix_create_transform {
    local(*vec1, *vec2, *vec3) = @_;
    local(@result);
    $#result = 8;
    for $i (0..2) {
        $result[$i*3+0] = $vec1[$i];
        $result[$i*3+1] = $vec2[$i];
        $result[$i*3+2] = $vec3[$i];
    }
    return @result;
}

sub matrix_identity {
    return (1,0,0, 0,1,0, 0,0,1);
}

sub get_transform_from_normal {
    local(*major, *normal) = @_;
    local(@result);
    local(@a, @b, @bprime);
    local(@z, @zprime);
    local(@first, @second);
    local(@zero_vector) = (0,0,0);
    @z = &vector_normalize(*major);
    @zprime = &vector_normalize(*normal);
    @a = &vector_cross_product(*z, *zprime);
    @a = &vector_normalize(*a);
    if (&vector_equal(*a, *zero_vector)) {
        return &matrix_identity();
    }
    @b = &vector_cross_product(*z, *a);
    @bprime = &vector_cross_product(*zprime, *a);
    @first = &matrix_create_transform(*a, *b, *z);
    @first = &matrix_transpose(*first);
    @second = &matrix_create_transform(*a, *bprime, *zprime);
    @result = &matrix_multiply(*second, *first);
    return @result;
}

sub get_direction_cosines {
    local($orientation, *normal, *col_dircos, *row_dircos, *slc_dircos) = @_;

    # Check for normal that is not in one of the major planes
    local($danger_normal) = 1;
    foreach $coord (@normal) {
        if ($coord == 0.0) { 
            $danger_normal = 0;
        }
    }
    if ($danger_normal) {
        warn "\n\nWARNING!!!!! ".
        "Found old Siemens file with dangerous image normal (2 rotations).\n".
        "Getting out of here fast! Time to fix this program!\n\n\n";
        &cleanup_and_die("Sorry!\n", -1);
    }

    # Get transformation from col,row,slice (axis) to x,y,z (world)
    local(@axis_to_world, @world_to_axis);
    if ($orientation eq "transverse") {
        @world_to_axis = (1,0,0, 0,1,0, 0,0,1);
    }
    elsif ($orientation eq "sagittal") {
        @world_to_axis = (0,1,0, 0,0,1, 1,0,0);
    }
    elsif ($orientation eq "coronal") {
        @world_to_axis = (1,0,0, 0,0,1, 0,1,0);
    }
    else {die "Unknown orientation \"$orientation\"";}
    @axis_to_world = &matrix_transpose(*world_to_axis);

    # Transform normal and set closest axis
    local(@new_normal) = &matrix_vector_multiply(*world_to_axis, *normal);
    local(@major) = (0,0,1);

    # Get transform
    local(@transform) = &get_transform_from_normal(*major, *new_normal);

    # Extract direction cosines
    local(@transpose) = &matrix_transpose(*transform);
    @col_dircos = @transpose[0..2];
    @row_dircos = @transpose[3..5];
    @slc_dircos = @transpose[6..8];

    # Transform the direction cosines back to x,y,z (world) coordinates
    @col_dircos = &matrix_vector_multiply(*axis_to_world, *col_dircos);
    @row_dircos = &matrix_vector_multiply(*axis_to_world, *row_dircos);
    @slc_dircos = &matrix_vector_multiply(*axis_to_world, *slc_dircos);

}

sub convert_coordinates {
    local(@coords) = @_;
    $coords[0] *= -1;
    $coords[2] *= -1;
    return @coords;
}

# NUMARIS 2 ROUTINES

# Subroutine to read the siemens Numaris 2 file headers
sub numaris2_read_headers {

    # Set constants for reading file
    local($header_off) = 0;
    local($header_len) = 8192;
    local($magic_off) = 244+42;
    local($magic_len) = 12;
    local($magic_value) = "SIEMENS MED ";


    # Check arguements
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in numaris2_read_headers",1);
    }
    local($filename, *header) = @_;

    # Open the file
    if (!open(SMNF, "<".$filename)) {
        warn "Can't open file $filename: $!";
        return 1;
    }

    # Read in the header
    if (!seek(SMNF, $header_off, 0) ||
        (read(SMNF, $header, $header_len) != $header_len)) {
        return 1;
    }

    # Check the image file magic number
    if (substr($header, $magic_off, $magic_len) ne $magic_value) {
        warn "Bad image file magic number in \"$filename\"";
        return 1;
    }

    # Close input file
    close(SMNF);

    return 0;
}

# Routine to get Numaris 2 file info
sub numaris2_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local($header);
    if (&numaris2_read_headers($filename, *header)) {
        return 1;
    }

    # Get the file headers
    local($blksize) = 512;
    local($ident_hdr) = substr($header, 0, 244);
    local($relat_hdr) = substr($header, 1*$blksize+162, 166);
    local($patient_hdr) = substr($header, 6*$blksize, 512);
    local($meas_hdr) = substr($header, 7*$blksize, 3072);
    local($image_hdr) = substr($header, 13*$blksize, 512);

    # Get interesting values
    $file_info{'numechos'} = &unpack_int(*meas_hdr, 512);
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    # We cheat and use patient id for exam, getting rid of weird characters
    ($file_info{'exam'} = &unpack_value(*patient_hdr, 28, 'A12'))
        =~ s/^\W//g;
    $file_info{'series'} = &unpack_int(*patient_hdr, 112);
    $file_info{'image'} = &unpack_value(*relat_hdr, 56, 'A6') + 0;

    $file_info{'echo'} = &unpack_int(*meas_hdr, 516);
    $file_info{'width'} = &unpack_short(*image_hdr, 138);
    $file_info{'height'} = &unpack_short(*image_hdr, 140);
    $file_info{'pixel_size'} = 2;
    $file_info{'patient_name'} = &unpack_value(*patient_hdr, 0, "A26");
    local(@normal) = &convert_coordinates(&unpack_float(*meas_hdr, 380),
                                          &unpack_float(*meas_hdr, 384),
                                          &unpack_float(*meas_hdr, 388));
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
    local($fovx, $fovy);
    local(@col_dircos, @row_dircos, @slc_dircos);
    $fovx = &unpack_float(*meas_hdr, 400);
    $fovy = &unpack_float(*meas_hdr, 400);
    if ($fovy == 0) {$fovy = $fovx};
    $file_info{'colstep'} = -$fovx / $file_info{'width'};
    $file_info{'rowstep'} = -$fovy / $file_info{'height'};
    &get_direction_cosines($file_info{'orientation'}, *normal, 
                           *col_dircos, *row_dircos, *slc_dircos);
    @col_dircos = &get_dircos(@col_dircos);
    @row_dircos = &get_dircos(@row_dircos);
    @slc_dircos = &get_dircos(@slc_dircos);
    local($xcentre, $ycentre, $zcentre) = 
        &convert_coordinates(&unpack_float(*meas_hdr, 368),
                             &unpack_float(*meas_hdr, 372),
                             &unpack_float(*meas_hdr, 376));
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
    $file_info{'tr'} = &unpack_float(*meas_hdr, 268)/1000;
    $file_info{'te'} = &unpack_float(*meas_hdr, 520)/1000;
    $file_info{'ti'} = &unpack_float(*meas_hdr, 264)/1000;
    $file_info{'mr_flip'} = &unpack_int(*meas_hdr, 284);
    $file_info{'scan_seq'} = &unpack_value(*meas_hdr, 172, 'a8');
    $file_info{'scan_seq'} =~ s/\0.*$//;
    $file_info{'scan_seq'} =~ s/\n//;
    ($file_info{'patient_birthdate'} = &unpack_value(*patient_hdr, 40, 'a10'))
        =~ s/\./-/g;
    local($sex_flag) = &unpack_value(*patient_hdr, 52, 'a2');
    if ($sex_flag eq 'M_') {
        $file_info{'patient_sex'} = "male__";
    }
    elsif ($sex_flag eq 'F_') {
        $file_info{'patient_sex'} = "female";
    }
    $file_info{'patient_id'} = &unpack_value(*patient_hdr, 28, 'A12');
    $file_info{'institution'} = &unpack_value(*ident_hdr, 144, 'A26');
    local($study_date, $study_time);
    ($study_date = &unpack_value(*ident_hdr, 52, 'A10')) =~ s/\./-/g;
    $study_time = &unpack_value(*ident_hdr, 70, 'A14');
    $file_info{'start_time'} = "$study_date $study_time";

    # Get specific file info
    $specific_file_info{'pixel_data_offset'} = 8192;
    $specific_file_info{'pixel_data_len'} = 
        $file_info{'width'} * $file_info{'height'} * 2;

    return 0;
}

sub numaris2_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in numaris2_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " . 
        $specific_file_info{'pixel_data_offset'} . " " .
        $specific_file_info{'pixel_data_len'} . " " .
        $cur_file . " | byte_swap ";

    return $cmd;
}

# NUMARIS 3 ROUTINES

# GENERAL SIEMENS ROUTINES

# Routine to initialize tape drive
sub siemens_initialize_tape_drive {
}

# Routine to read the file info
sub siemens_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    local($numaris2_magic) = "SIEMENS MED ";
    local($numaris2_magic_id) = "0x0009 0x0011";
    local($numaris3_magic) = "SIEMENS CM VA0  CMS ";
    local($numaris3_magic_id) = "0x0009 0x0012";

    local($magic);

    # Check for a numaris 2 file
    $magic = `extract_acr_nema $filename $numaris2_magic_id 2>/dev/null`;
    if ($magic eq $numaris2_magic) {
        $specific_file_info{'siemens_file_type'} = "Numaris2";
        return &numaris2_read_file_info(@_);
    }

    # Check for a numaris 3 file
    $magic = `extract_acr_nema $filename $numaris3_magic_id 2>/dev/null`;
    if ($magic eq $numaris3_magic) {
        $specific_file_info{'siemens_file_type'} = "Numaris3";
        return &numaris3_read_file_info(@_);
    }

    # Can't figure out file type
    warn "Unable to identify siemens file type in \"$filename\"";
    return 1;

}

# Routine to get the image
sub siemens_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in siemens_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    if ($specific_file_info{'siemens_file_type'} eq "Numaris2") {
        return &numaris2_get_image_cmd(@_);
    }
    elsif ($specific_file_info{'siemens_file_type'} eq "Numaris3") {
        return &numaris2_get_image_cmd(@_);
    }
    else {
        die "Unknown siemens file type";
    }
}

# MAIN PROGRAM

*initialize_tape_drive = *siemens_initialize_tape_drive;
*read_file_info = *siemens_read_file_info;
*get_image_cmd = *siemens_get_image_cmd;

&mri_to_minc(@ARGV);

