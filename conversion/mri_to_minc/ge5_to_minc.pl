# Subroutines for GE mri machines, version 5.x

# Routine to initialize tape drive
sub ge5_initialize_tape_drive {
    local($tapedrive) = @_;

    local($nextfile) = &read_next_file($tapedrive);
    if ($delete_files) {
        &remove_file($nextfile);
    }
    
}

# Subroutine to read the ge 5.x file headers
sub ge5_read_headers {

    # Set constants for reading file
    local($suite_hdr_len) = 114;
    local($exam_hdr_len) = 1024;
    local($series_hdr_len) = 1020;
    local($image_hdr_len) = 1022;
    local($pixel_hdr_prefix_len) = 124;
    local($pixdatsize_len) = 4;

    # Check arguements
    if (scalar(@_) != 8) {
        &cleanup_and_die("Argument error in ge5_read_headers",1);
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

    # Check the image file magic number
    if (unpack("a4",$pixel_hdr) ne "IMGF") {
        warn "Bad image file magic number in \"$filename\"";
        return 1;
    }

    # Get the length of the pixel header
    local($pixel_hdr_len) = unpack("L", substr($pixel_hdr, 4, 4));

    # Calculate the offset to pixel data
    local($pixdatsize_offset) = $suite_hdr_len + $exam_hdr_len + 
        $series_hdr_len + $image_hdr_len + $pixel_hdr_len;
    $pixel_data_offset = $pixdatsize_offset + $pixdatsize_len;

    # Read in the length of the pixel data
    local($pixdatsize);
    seek(GEF, $pixdatsize_offset, 0);
    return 1
        if (read(GEF, $pixdatsize, $pixdatsize_len) != $pixdatsize_len);
    $pixel_data_len = unpack("L",$pixdatsize);

    # Close input file
    close(GEF);

    return 0;
}

# Routine to get file info
sub ge5_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local($suite_hdr, $exam_hdr, $series_hdr, $image_hdr, $pixel_hdr,
          $pixel_data_offset, $pixel_data_len);
    if (&ge5_read_headers($filename, *suite_hdr, *exam_hdr, *series_hdr,
                          *image_hdr, *pixel_hdr, 
                          *pixel_data_offset, *pixel_data_len)) {
        return 1;
    }

    # Get interesting values
    local($fovx, $fovy);
    $file_info{'numechos'} = &unpack_value(*image_hdr, 210, 's');
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    $file_info{'exam'} = &unpack_value(*image_hdr, 8, 'S');
    $file_info{'series'} = &unpack_value(*image_hdr, 10, 's');
    $file_info{'image'} = &unpack_value(*image_hdr, 12, 's');
    $file_info{'echo'} = &unpack_value(*image_hdr, 212, 's');
    $file_info{'width'} = &unpack_value(*pixel_hdr, 8, 'i');
    $file_info{'height'} = &unpack_value(*pixel_hdr, 12, 'i');
    $file_info{'pixel_size'} = 2;
    $file_info{'slicepos'} = &unpack_value(*image_hdr, 126, 'f');
    $file_info{'patient_name'} = &unpack_value(*exam_hdr, 97, "A25");
    local($orient_flag) = &unpack_value(*image_hdr, 114, 's');
    if ($orient_flag == 2) {    # Transverse
        $file_info{'orientation'} = 'transverse';
    }
    elsif ($orient_flag == 4) {    # Sagittal
        $file_info{'orientation'} = 'sagittal';
    }
    elsif (($orient_flag == 8) ||
           ($orient_flag == 16)) { # Coronal
        $file_info{'orientation'} = 'coronal';
    }
    else {                      # Assume transverse
        print STDERR "orient_flag = $orient_flag, assuming transverse\n";
        $file_info{'orientation'} = 'transverse';
    }

    $fovx = &unpack_value(*image_hdr, 34, 'f');
    $fovy = &unpack_value(*image_hdr, 38, 'f');
    $file_info{'colstep'} = -$fovx / $file_info{'width'};
    $file_info{'rowstep'} = 
        -(($fovy != 0) ? $fovy : $fovx) / $file_info{'height'};
    $file_info{'tr'} = &unpack_value(*image_hdr, 194, 'i')/1000000;
    $file_info{'te'} = &unpack_value(*image_hdr, 202, 'i')/1000000;
    $file_info{'ti'} = &unpack_value(*image_hdr, 198, 'i')/1000000;
    $file_info{'mr_flip'} = &unpack_value(*image_hdr, 254, 's');
    $file_info{'scan_seq'} = &unpack_value(*image_hdr, 308, 'a33');
    $file_info{'scan_seq'} =~ s/\0.*$//;
    $file_info{'scan_seq'} =~ s/\n//;
    $file_info{'ge_pseq'} = &unpack_value(*image_hdr, 304, 's');
    $file_info{'ge_pseqmode'} = &unpack_value(*image_hdr, 306, 's');

    # Get GE specific stuff
    $specific_file_info{'pixel_data_offset'} = $pixel_data_offset;
    $specific_file_info{'pixel_data_len'} = $pixel_data_len;
    local($compress) = &unpack_value(*pixel_hdr, 20, 'i');
    $specific_file_info{'compress'} = $compress;
    if (($compress != 1) && ($compress != 3)) {
        warn "Unusable compression scheme ($compress).";
        return 1;
    }
    local($depth) = &unpack_value(*pixel_hdr, 16, 'i');
    $specific_file_info{'depth'} = $depth;
    if ($depth != 16) {
        warn "Unusable pixel depth ($depth).";
        return 1;
    }
    
    return 0;
}

sub ge5_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in ge5_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " . 
        $specific_file_info{'pixel_data_offset'} . " " .
        $specific_file_info{'pixel_data_len'} . " " .
        $cur_file;

    if ($specific_file_info{'compress'} == 3) {
        $cmd .= " | ge_uncompress";
    }

    return $cmd;
}

# MAIN PROGRAM

*initialize_tape_drive = *ge5_initialize_tape_drive;
*read_file_info = *ge5_read_file_info;
*get_image_cmd = *ge5_get_image_cmd;

&mri_to_minc(@ARGV);

