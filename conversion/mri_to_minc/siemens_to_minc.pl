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

# Routine to initialize tape drive
sub siemens_initialize_tape_drive {
}

# Subroutine to read the siemens file headers
sub siemens_read_headers {

    # Set constants for reading file
    local($blksize) = 512;
    local($ident_hdr_off) = 0;
    local($ident_hdr_len) = 244;
    local($magic_off) = 244+42;
    local($magic_len) = 12;
    local($patient_hdr_off) = 6*$blksize;
    local($patient_hdr_len) = 512;
    local($meas_hdr_off) = 7*$blksize;
    local($meas_hdr_len) = 3072;
    local($image_hdr_off) = 13*$blksize;
    local($image_hdr_len) = 512;
    local($magic_value) = "SIEMENS MED ";

    # Check arguements
    if (scalar(@_) != 5) {
        &cleanup_and_die("Argument error in siemens_read_headers",1);
    }
    local($filename, *ident_hdr, *patient_hdr, *meas_hdr, *image_hdr) = @_;
    local($magic);

    # Open the file
    if (!open(SMNF, "<".$filename)) {
        warn "Can't open file $filename: $!";
        return 1;
    }

    # Check the image file magic number
    if (!seek(SMNF, $magic_off, 0) || 
        (read(SMNF, $magic, $magic_len) != $magic_len) ||
        ($magic ne $magic_value)) {
        warn "Bad image file magic number in \"$filename\"";
        return 1;
    }

    # Read in file headers
    return 1
        if (!seek(SMNF, $ident_hdr_off, 0) || 
            (read(SMNF, $ident_hdr, $ident_hdr_len) != $ident_hdr_len));
    return 1
        if (!seek(SMNF, $patient_hdr_off, 0) || 
            (read(SMNF, $patient_hdr, $patient_hdr_len) != $patient_hdr_len));
    return 1
        if (!seek(SMNF, $meas_hdr_off, 0) || 
            (read(SMNF, $meas_hdr, $meas_hdr_len) != $meas_hdr_len));
    return 1
        if (!seek(SMNF, $image_hdr_off, 0) || 
            (read(SMNF, $image_hdr, $image_hdr_len) != $image_hdr_len));

    # Close input file
    close(SMNF);

    return 0;
}

# Routine to get file info
sub siemens_read_file_info {
    if (scalar(@_) != 3) {
        &cleanup_and_die("Argument error in read_file_info",1);
    }
    local($filename, *file_info, *specific_file_info) = @_;

    # Get headers
    local($ident_hdr, $patient_hdr, $meas_hdr, $image_hdr);
    if (&siemens_read_headers($filename, *ident_hdr, *patient_hdr, 
                              *meas_hdr, *image_hdr)) {
        return 1;
    }

    # Get interesting values
    local($fovx, $fovy);
    $file_info{'numechos'} = &unpack_int(*meas_hdr, 512);
    if ($file_info{'numechos'} <= 0) {$file_info{'numechos'} = 1;}
    # We cheat and use patient id for exam, getting rid of weird characters
    ($file_info{'exam'} = &unpack_value(*patient_hdr, 28, 'A12'))
        =~ s/[^\w]//g;
    $file_info{'series'} = &unpack_int(*patient_hdr, 112);
    $file_info{'image'} = &unpack_int(*meas_hdr, 364);
    $file_info{'echo'} = &unpack_int(*meas_hdr, 516);
    $file_info{'width'} = &unpack_short(*image_hdr, 138);
    $file_info{'height'} = &unpack_short(*image_hdr, 140);
    $file_info{'pixel_size'} = 2;
    $file_info{'slicepos'} = &unpack_float(*meas_hdr, 404);
    $file_info{'patient_name'} = &unpack_value(*patient_hdr, 0, "A26");
    local($norm_r) = &unpack_float(*meas_hdr, 380);
    local($norm_a) = &unpack_float(*meas_hdr, 384);
    local($norm_s) = &unpack_float(*meas_hdr, 388);
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
    $fovx = &unpack_float(*meas_hdr, 400);
    $fovy = &unpack_float(*meas_hdr, 400);
    $file_info{'colstep'} = -$fovx / $file_info{'width'};
    $file_info{'rowstep'} = 
        -(($fovy != 0) ? $fovy : $fovx) / $file_info{'height'};
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

sub siemens_get_image_cmd {
    if (scalar(@_) != 2) {
        &cleanup_and_die("Argument error in siemens_get_image_cmd",1);
    }
    local($cur_file, *specific_file_info) = @_;

    local($cmd) = 
        "extract " . 
        $specific_file_info{'pixel_data_offset'} . " " .
        $specific_file_info{'pixel_data_len'} . " " .
        $cur_file . " | byte_swap ";

    return $cmd;
}

# MAIN PROGRAM

*initialize_tape_drive = *siemens_initialize_tape_drive;
*read_file_info = *siemens_read_file_info;
*get_image_cmd = *siemens_get_image_cmd;

&mri_to_minc(@ARGV);

