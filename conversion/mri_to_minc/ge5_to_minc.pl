# Subroutines for GE mri machines, version 5.x

# Routine to convert unix time to date string
sub time_to_string {
    if (scalar(@_) != 1) {
        die "Argument error in time_to_string\n";
    }
    local(@months) = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
                      "Aug", "Sep", "Oct", "Nov", "Dec");
    local($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)
        = gmtime(@_[0]);
    if ($year >= 70) {$year += 1900;}
    else {$year += 2000;}

    return sprintf("%02d-%s-%04d %02d:%02d:%02d GMT", 
                   $mday, $months[$mon], $year, $hour, $min, $sec);
}

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

    # Check arguements
    if (scalar(@_) != 8) {
        &cleanup_and_die("Argument error in ge5_read_headers",1);
    }
    local($filename, *suite_hdr, *exam_hdr, *series_hdr, 
          *image_hdr, *pixel_hdr, *pixel_data_offset, *pixel_data_len)
        = @_;

    # Set constants for reading DAT files
    local($magic_string) = "IMGF";
    local($suite_hdr_len) = 114;
    local($suite_hdr_off) = 0;
    local($exam_hdr_len) = 1024;
    local($exam_hdr_off) = $suite_hdr_off + $suite_hdr_len;
    local($series_hdr_len) = 1020;
    local($series_hdr_off) = $exam_hdr_off + $exam_hdr_len;
    local($image_hdr_len) = 1022;
    local($image_hdr_off) = $series_hdr_off + $series_hdr_len;
    local($DAT_pixel_hdr_prefix_len) = 124;
    local($DAT_pixel_hdr_off) = $image_hdr_off + $image_hdr_len;
    local($DAT_pixdatsize_len) = 4;

    # Set pixel header constants for disk files
    local($DISK_pixel_hdr_prefix_len) = 156;
    local($DISK_pixel_hdr_off) = 0;

    # Open file
    if (!open(GEF, "<".$filename)) {
        warn "Can't open file $filename: $!\n";
        return 1;
    }

    # Check to see if this is a disk file or DAT file
    return 1
       if (read(GEF, $pixel_hdr, $DISK_pixel_hdr_prefix_len) 
           != $DISK_pixel_hdr_prefix_len);
    local($is_DISK_file) = (substr($pixel_hdr, 0, 4) eq $magic_string);

    # For disk file, read header positions and lengths
    if ($is_DISK_file) {
       $suite_hdr_off = &unpack_value(*pixel_hdr, 124, 'i');
       $suite_hdr_len = &unpack_value(*pixel_hdr, 128, 'i');
       $exam_hdr_off = &unpack_value(*pixel_hdr, 132, 'i');
       $exam_hdr_len = &unpack_value(*pixel_hdr, 136, 'i');
       $series_hdr_off = &unpack_value(*pixel_hdr, 140, 'i');
       $series_hdr_len = &unpack_value(*pixel_hdr, 144, 'i');
       $image_hdr_off = &unpack_value(*pixel_hdr, 148, 'i');
       $image_hdr_len = &unpack_value(*pixel_hdr, 152, 'i');
       $pixel_data_offset = &unpack_value(*pixel_hdr, 4, 'i');
       $pixel_data_len = -1;
    }

    # For DAT file read in pixel header and get image position and length
    else {
       return 1 if (!seek(GEF, $DAT_pixel_hdr_off, 0));
       return 1
          if (read(GEF, $pixel_hdr, $DAT_pixel_hdr_prefix_len) 
              != $DAT_pixel_hdr_prefix_len);

       # Check the image file magic number
       if (substr($pixel_hdr, 0, 4) ne $magic_string) {
          warn "Bad image file magic number in \"$filename\"\n";
          return 1;
       }

       # Get the length of the pixel header
       local($pixel_hdr_len) = &unpack_value(*pixel_hdr, 4, "i");

       # Calculate the offset to pixel data
       local($pixdatsize_len) = $DAT_pixdatsize_len;
       local($pixdatsize_offset) = $suite_hdr_len + $exam_hdr_len + 
          $series_hdr_len + $image_hdr_len + $pixel_hdr_len;
       $pixel_data_offset = $pixdatsize_offset + $pixdatsize_len;

       # Read in the length of the pixel data
       local($pixdatsize);
       return 1 if (!seek(GEF, $pixdatsize_offset, 0));
       return 1
          if (read(GEF, $pixdatsize, $pixdatsize_len) != $pixdatsize_len);
       $pixel_data_len = &unpack_value(*pixdatsize, 0, "i");

    }

    # Read in file headers
    return 1 if (!seek(GEF, $suite_hdr_off, 0));
    return 1
        if (read(GEF, $suite_hdr, $suite_hdr_len) != $suite_hdr_len);
    return 1 if (!seek(GEF, $exam_hdr_off, 0));
    return 1
        if (read(GEF, $exam_hdr, $exam_hdr_len) != $exam_hdr_len);
    return 1 if (!seek(GEF, $series_hdr_off, 0));
    return 1
        if (read(GEF, $series_hdr, $series_hdr_len) != $series_hdr_len);
    return 1 if (!seek(GEF, $image_hdr_off, 0));
    return 1
        if (read(GEF, $image_hdr, $image_hdr_len) != $image_hdr_len);

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
    $file_info{'numechos'} = &unpack_value(*image_hdr, 210, 's');
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    $file_info{'exam'} = &unpack_value(*image_hdr, 8, 'S');
    $file_info{'series'} = &unpack_value(*image_hdr, 10, 's');
    $file_info{'image'} = &unpack_value(*image_hdr, 12, 's');
    $file_info{'echo'} = &unpack_value(*image_hdr, 212, 's');
    $file_info{'width'} = &unpack_value(*pixel_hdr, 8, 'i');
    $file_info{'height'} = &unpack_value(*pixel_hdr, 12, 'i');
    $file_info{'pixel_size'} = 2;
    $file_info{'patient_name'} = &unpack_value(*exam_hdr, 97, "A25");
    local($orient_flag) = &unpack_value(*image_hdr, 114, 's');
    if ($orient_flag == 2) {    # Transverse
        $file_info{'orientation'} = 'transverse';
    }
    elsif ($orient_flag == 4) {    # Sagittal
        $file_info{'orientation'} = 'sagittal';
    }
    elsif ($orient_flag == 8) { # Coronal
        $file_info{'orientation'} = 'coronal';
    }
    elsif ($orient_flag == 16) { # Oblique (check normal vector)
        local($norm_r, $norm_a, $norm_s) = 
            &abs(&unpack_value(*image_hdr, 142, 'f3'));
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
    }
    else {                      # Assume transverse
        print STDERR "orient_flag = $orient_flag, assuming transverse\n";
        $file_info{'orientation'} = 'transverse';
    }

    # Crude hack to look for multiple scans with identical series number
    # Check for new exam/series to reset values. If we have the same
    # series, then check whether the raw data run number has changed.
    # If so, then increment the runid and modify the series string that
    # we are using
    # Uses globals $GE5_CUR{EXAM,SERIES,RUN} and $GE5_{SERIESID,RUNID}
    local($thisrun) = &unpack_value(*image_hdr, 383, 'i');
    if (!defined($GE5_CUREXAM) ||
        ($file_info{'exam'} != $GE5_CUREXAM) ||
        ($file_info{'series'} != $GE5_CURSERIES)) {
       $GE5_CUREXAM = $file_info{'exam'};
       $GE5_CURSERIES = $file_info{'series'};
       $GE5_CURRUN = $thisrun;
       $GE5_RUNID = '';
       $GE5_SERIESID = $GE5_CURSERIES;
    }
    elsif ($thisrun != $GE5_CURRUN) {
       $GE5_CURRUN = $thisrun;
       $GE5_RUNID++;
       $GE5_SERIESID = $GE5_CURSERIES . "." . $GE5_RUNID;
    }
    $file_info{'series'} = $GE5_SERIESID;

    # Get coordinate information
    local($fovx, $fovy);
    $fovx = &unpack_value(*image_hdr, 34, 'f');
    $fovy = &unpack_value(*image_hdr, 38, 'f');
    if ($fovy == 0) {$fovy = $fovx};
    $file_info{'colstep'} = -$fovx / $file_info{'width'};
#   Being fancy doesn't work - just assume square pixels
#    $file_info{'rowstep'} = -$fovy / $file_info{'height'};
    $file_info{'rowstep'} = $file_info{'colstep'};
    local($xcentre, $ycentre, $zcentre) = 
        &unpack_value(*image_hdr, 130, 'f3');
    local($xnorm, $ynorm, $znorm) = 
        &unpack_value(*image_hdr, 142, 'f3');
    local($xtoplh, $ytoplh, $ztoplh) = 
        &unpack_value(*image_hdr, 154, 'f3');
    local($xtoprh, $ytoprh, $ztoprh) = 
        &unpack_value(*image_hdr, 166, 'f3');
    local($xbotrh, $ybotrh, $zbotrh) = 
        &unpack_value(*image_hdr, 178, 'f3');
    local($xnorm, $ynorm, $znorm) = &get_dircos($xnorm, $ynorm, $znorm);
    local($x_col_dircos, $y_col_dircos, $z_col_dircos) =
        &get_dircos($xtoprh - $xtoplh, $ytoprh - $ytoplh, $ztoprh - $ztoplh);
    local($x_row_dircos, $y_row_dircos, $z_row_dircos) =
        &get_dircos($xbotrh - $xtoprh, $ybotrh - $ytoprh, $zbotrh - $ztoprh);
    $file_info{'slicepos'} = 
        $xcentre * $xnorm + $ycentre * $ynorm + $zcentre * $znorm;
    $file_info{'colstart'} = 
        ($xcentre * $x_col_dircos + 
         $ycentre * $y_col_dircos + 
         $zcentre * $z_col_dircos) - 
             $file_info{'colstep'} * ($file_info{'width'} - 1) / 2;
    $file_info{'rowstart'} = 
        ($xcentre * $x_row_dircos + 
         $ycentre * $y_row_dircos + 
         $zcentre * $z_row_dircos) -
             $file_info{'rowstep'} * ($file_info{'height'} - 1) / 2;
    $file_info{'col_dircos_x'} = $x_col_dircos;
    $file_info{'col_dircos_y'} = $y_col_dircos;
    $file_info{'col_dircos_z'} = $z_col_dircos;
    $file_info{'row_dircos_x'} = $x_row_dircos;
    $file_info{'row_dircos_y'} = $y_row_dircos;
    $file_info{'row_dircos_z'} = $z_row_dircos;
    $file_info{'slc_dircos_x'} = $xnorm;
    $file_info{'slc_dircos_y'} = $ynorm;
    $file_info{'slc_dircos_z'} = $znorm;

    # Get other info
    $file_info{'tr'} = &unpack_value(*image_hdr, 194, 'i')/1000000;
    $file_info{'te'} = &unpack_value(*image_hdr, 202, 'i')/1000000;
    $file_info{'ti'} = &unpack_value(*image_hdr, 198, 'i')/1000000;
    $file_info{'mr_flip'} = &unpack_value(*image_hdr, 254, 's');
    $file_info{'scan_seq'} = &unpack_value(*image_hdr, 308, 'a33');
    $file_info{'scan_seq'} =~ s/\0.*$//;
    $file_info{'scan_seq'} =~ s/\n//;
    $file_info{'ge_pseq'} = &unpack_value(*image_hdr, 304, 's');
    $file_info{'ge_pseqmode'} = &unpack_value(*image_hdr, 306, 's');
    $file_info{'patient_age'} = &unpack_value(*exam_hdr, 122, 's');
    local($sex_flag) = &unpack_value(*exam_hdr, 126, 's');
    if ($sex_flag == 1) {
        $file_info{'patient_sex'} = "male__";
    }
    elsif ($sex_flag == 2) {
        $file_info{'patient_sex'} = "female";
    }
    $file_info{'patient_id'} = &unpack_value(*exam_hdr, 84, 'A13');
    $file_info{'institution'} = &unpack_value(*exam_hdr, 10, 'A33');
    $file_info{'start_time'} = 
        &time_to_string(&unpack_value(*image_hdr, 14, 'i'));

    # Get GE specific stuff
    $specific_file_info{'pixel_data_offset'} = $pixel_data_offset;
    $specific_file_info{'pixel_data_len'} = $pixel_data_len;
    local($compress) = &unpack_value(*pixel_hdr, 20, 'i');
    $specific_file_info{'compress'} = $compress;
    if (($compress != 1) && ($compress != 3)) {
        warn "Unusable compression scheme ($compress).\n";
        return 1;
    }
    local($depth) = &unpack_value(*pixel_hdr, 16, 'i');
    $specific_file_info{'depth'} = $depth;
    if ($depth != 16) {
        warn "Unusable pixel depth ($depth).\n";
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

