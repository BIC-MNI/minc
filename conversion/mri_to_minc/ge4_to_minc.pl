# Subroutines for GE mri machines, version 4.x

# Routine to initialize tape drive
sub ge4_initialize_tape_drive {
   &cleanup_and_die("ge4_to_minc does not support tape reading.\n",1);
}

# Routine to convert data general float to ieee
sub convert_dg_float_to_ieee {
    local($packed_float, $ival, $exponent, $mantissa, @result, $int_result);
    @result = ();
    foreach $packed_float (@_) {
        $ival = unpack('L',$packed_float);
        if ($ival == 0.0) {
            push(@result, 0.0);
        }
        else {
            $exponent = (((($ival >> 24) & 0x7f) - 64) * 4) + 127;
            $mantissa = $ival & 0x00ffffff;
            while ($mantissa && (!( $mantissa & 0x00800000))) {
                $exponent--;
                $mantissa = $mantissa << 1;
            }
            $exponent--;
            $exponent = $exponent << 23;
            $mantissa &= 0x007fffff;       

            $int_result = ($ival & 0x80000000) | $exponent | $mantissa;
            push(@result, unpack('f',pack('L', $int_result)));
        }
                                      
    }
    return (scalar(@result) > 1) ? @result : $result[0];
}

# Subroutine to unpack a data general float from a string
sub unpack_float {
    local(*string, $offset) = @_;
    if ($dump_unpack_value) {
        print "unpack_value: length, offset = ",length($string), 
        " $offset\n";
    }
    return &convert_dg_float_to_ieee(substr($string, $offset, 4));
}

# Subroutine to read the ge 4.x file headers
sub ge4_read_headers {

    # Set constants for reading file
    local($suite_hdr_len) = 3072;
    local($exam_hdr_len) = 1024;
    local($series_hdr_len) = 1024;
    local($image_hdr_len) = 1024;
    local($pixel_hdr_prefix_len) = 2048;

    # Check arguments
    if (scalar(@_) != 8) {
        &cleanup_and_die("Argument error in ge4_read_headers",1);
    }
    local($filename, *suite_hdr, *exam_hdr, *series_hdr, 
          *image_hdr, *pixel_hdr, *pixel_data_offset, *pixel_data_len)
        = @_;

    # Read in file headers
    if (!open(GEF, "<".$filename)) {
        warn "Can't open file $filename: $!";
        return 1;
    }
    return 1
        if (read(GEF, $suite_hdr, $suite_hdr_len) != $suite_hdr_len);
    return 1
        if (read(GEF, $exam_hdr, $exam_hdr_len) != $exam_hdr_len);
    return 1
        if (read(GEF, $series_hdr, $series_hdr_len) != $series_hdr_len);
    return 1
        if (read(GEF, $image_hdr, $image_hdr_len) != $image_hdr_len);
    return 1
        if (read(GEF, $pixel_hdr, $pixel_hdr_prefix_len) !=
            $pixel_hdr_prefix_len);

    # Calculate the offset to pixel data
    $pixel_data_offset = 14336;

    # Use -1 for pixel data length to get to end of file
    $pixel_data_len = -1;

    # Close input file
    close(GEF);

    return 0;
}

# Routine to get file info
sub ge4_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local($suite_hdr, $exam_hdr, $series_hdr, $image_hdr, $pixel_hdr,
          $pixel_data_offset, $pixel_data_len);
    if (&ge4_read_headers($filename, *suite_hdr, *exam_hdr, *series_hdr,
                          *image_hdr, *pixel_hdr, 
                          *pixel_data_offset, *pixel_data_len)) {
        return 1;
    }

    # Get interesting values
    local($fovx, $fovy);
    $file_info{'numechos'} = &unpack_value(*image_hdr, 196, 's');
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    $file_info{'exam'} = &unpack_value(*exam_hdr, 64, 'A6')+0;
    $file_info{'series'} = &unpack_value(*image_hdr, 92, 'A4')+0;
    $file_info{'image'} = &unpack_value(*image_hdr, 88, 'A4')+0;
    $file_info{'echo'} = &unpack_value(*image_hdr, 198, 's');
    $file_info{'width'} = &unpack_value(*image_hdr, 274, 's');
    $file_info{'height'} = &unpack_value(*image_hdr, 276, 's');
    $file_info{'pixel_size'} = 2;
    $file_info{'slicepos'} = &unpack_float(*image_hdr, 146);
    $file_info{'patient_name'} = &unpack_value(*exam_hdr, 108, "A25");
    local($orient_flag) = &unpack_value(*series_hdr, 276, 's');
    if ($orient_flag == 1) {    # Sagittal
        $file_info{'orientation'} = 'sagittal';
    }
    elsif ($orient_flag == 2) { # Coronal
        $file_info{'orientation'} = 'coronal';
    }
    else {                      # Transverse
        $file_info{'orientation'} = 'transverse';
    }

    $fovx = &unpack_float(*series_hdr, 302);
    $fovy = $fovx;
    $file_info{'colstep'} = -$fovx / $file_info{'width'};
    $file_info{'rowstep'} = 
        -(($fovy != 0) ? $fovy : $fovx) / $file_info{'height'};
    $file_info{'tr'} = &unpack_float(*image_hdr, 164)/1000000;
    $file_info{'te'} = &unpack_float(*image_hdr, 172)/1000000;
    $file_info{'ti'} = &unpack_float(*image_hdr, 176)/1000000;
    $file_info{'mr_flip'} = &unpack_value(*image_hdr, 350, 's');
    $file_info{'scan_seq'} = &unpack_value(*series_hdr, 410, 'A12');
    $file_info{'scan_seq'} =~ s/\0.*$//;
    $file_info{'scan_seq'} =~ s/\n//;
    $file_info{'ge_pseq'} = &unpack_value(*series_hdr, 298, 's');
    $file_info{'ge_pseqmode'} = &unpack_value(*series_hdr, 408, 's');

    # Get GE specific stuff
    $specific_file_info{'pixel_data_offset'} = $pixel_data_offset;
    $specific_file_info{'pixel_data_len'} = $pixel_data_len;
    local($compress) = &unpack_value(*image_hdr, 282, 's');
    $specific_file_info{'compress'} = $compress;
    if ($compress != 0) {
        warn "Unusable compression scheme ($compress).";
        return 1;
    }
    local($depth) = &unpack_value(*image_hdr, 284, 's');
    if ($depth <= 0) {$depth = 16;}
    $specific_file_info{'depth'} = $depth;
    if ($depth != 16) {
        warn "Unusable pixel depth ($depth).";
        return 1;
    }
    
    return 0;
}

sub ge4_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in ge4_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " . 
        $specific_file_info{'pixel_data_offset'} . " " .
        $specific_file_info{'pixel_data_len'} . " " .
        $cur_file;

    return $cmd;
}

# MAIN PROGRAM

*initialize_tape_drive = *ge4_initialize_tape_drive;
*read_file_info = *ge4_read_file_info;
*get_image_cmd = *ge4_get_image_cmd;

&mri_to_minc(@ARGV);

