#! /usr/local/bin/perl
#
# Routines for converting mri files to minc format. Must be included
# with routines for reading in data structures, specific to the input
# file format.
#

sub numeric_order { $a <=> $b;}

# Subroutine to clean up files and exit
sub cleanup_and_die {

    # Get message to print and exit status
    local($message,$status) = @_;
    if (!defined($status)) {$status = 0;}
    if (defined($message)) {
        print STDERR $message;
        if ($message !~ /\n$/) {print STDERR "\n";}
    }

    $SIG{'INT'}  = 'IGNORE';
    $SIG{'TERM'} = 'IGNORE';
    $SIG{'QUIT'} = 'IGNORE';
    # Check for temp files
    if (defined($tmpdir) && -e $tmpdir) {
        print STDERR "Cleaning up temporary files.\n";
        system "rm -rf $tmpdir";
    }

    exit($status);
}

# Subroutine to execute commands and check return status
sub execute {
    local($status);
    if ($print_execution_statements) {
        print join(' ',@_),"\n";
    }
    $status = system @_;
    if ($status != 0) {
        &cleanup_and_die("Error executing command \"".join(" ",@_)."\".\n",
                         $status);
    }
}

# Subroutine to remove a list of files
sub remove_file {
    unlink @_;
}

# Subroutine to create a temporary directory (global variable $tmpdir
# is created)
sub create_tmpdir {
    local($subdir);
    if (scalar(@_) >= 1) {
        $subdir = $_[0];
    }
    else {
        $subdir = $$;
    }
    if (defined($ENV{TMPDIR})) {
        $tmpdir = $ENV{TMPDIR};
    }
    else {
        $tmpdir = "/usr/tmp";
    }
    $tmpdir .= "/$subdir";
    mkdir($tmpdir,0777) || die "Unable to create directory $tmpdir: $!";
    $SIG{'INT'}  = 'cleanup_and_die';
    $SIG{'TERM'} = 'cleanup_and_die';
    $SIG{'QUIT'} = 'cleanup_and_die';
}

# Subroutine to read a file from tape
sub read_next_file {
    local($tapedrive, *input_list) = @_;
    if (!defined($tapedrive) && !defined(@input_list)) {
        $tapedrive = "/dev/tape";
    }

    # Constants
    $tape_block_size = 8192;
    $tape_sleep = 1;
    $retry_sleep = 2;
    $nretries = 4;
    $nreposition_retries = 4;


    # Get next value from list if no tape drive
    if (length($tapedrive) == 0) {
        return shift(@input_list);
    }

    # Create file counting variable if it does not exist and rewind tape
    # drive
    if (!defined($counter_for_read_next_file)) {
        print STDERR "Rewinding tape drive $tapedrive\n";
        &execute("mt -t $tapedrive rewind");
        $counter_for_read_next_file = 0;
    }

    # Create the filename
    local($cur_file_number) = $counter_for_read_next_file;
    $counter_for_read_next_file++;
    local($filename) = "$tmpdir/datafile_".$cur_file_number;

    # Try reading from the tape drive. We will try repeatedly if necessary
    print STDERR "Retrieving file $filename from drive $tapedrive\n";
    local($status);
    foreach $retryloop (0..$nretries-1) {

        # Sleep for a moment, then read from tape drive (don't ask me why,
        # it just works!)
        select(undef, undef, undef, $tape_sleep);
        $status = system("dd if=$tapedrive of=$filename ".
                         "bs=$tape_block_size >/dev/null 2>/dev/null");
        if ($status == 0) {last;}

        # If we get to here then the read failed. Try to reposition the tape.
        # We'll try a few times just in case. Throw in a sleep before 
        # rewinding for good measure.
        print STDERR "Error reading from tape - trying again.\n";
        local($tmp_status);
        foreach $reposloop (0..$nreposition_retries-1) {
            select(undef, undef, undef, $retry_sleep);
            $tmp_status = system("mt -t $tapedrive rewind");
            $tmp_status = system("mt -t $tapedrive fsf $cur_file_number")
                unless ($tmp_status != 0);
            if ($tmp_status == 0) {last;}
            print STDERR "Error repositioning tape - trying again.\n";
        }
        if ($tmp_status != 0) {
            warn "\n\nWARNING!!!!! Unable to reposition the tape.\n\n";
            last;
        }

        # Sleep to let things settle after repositioning
        select(undef, undef, undef, $retry_sleep);
        
    }

    # We've finished trying to read. Test to see if we failed or if
    # we've reached the end of the tape.
    if (($status!=0) || -z $filename) {
        if ($status != 0) {
            warn "\n\nWARNING!!!!! ".
                "Error occurred while reading tape. Giving up.\n\n\n";
            &cleanup_and_die("Not creating last minc file.\n", $status);
        }
        else {
            print STDERR "End of tape.\n";
        }
        &remove_file($filename);
        return "";
    }
    else {
        return $filename;
    }

}

# Subroutine to unpack a value from a string
sub unpack_value {
    local(*string, $offset, $type) = @_;
    return unpack("x$offset $type", $string);
}

# Subroutine to add an optional attribute to an array
sub add_optional_attribute {
    if (scalar(@_) != 3) {
        die "Argument error in add_optional_attribute\n";
    }
    local(*array, $name, $value) = @_;

    if (length($value) > 0) {
        push(@array, "-attribute", $name."=".$value);
    }
}

# Subroutine to create a minc file
sub create_mincfile {
    if (scalar(@_) != 5) {
        die "Argument error in create_mincfile";
    }
    local($mincfile, *file_list, *mincinfo, *image_list, $echo) 
        = @_;

    # Sort images by z position
    local($cur_image, %pos_to_image, @positions, $cur_slicepos);
    foreach $cur_image (split($;, $image_list{$echo})) {
        $cur_slicepos = $mincinfo{$cur_image, 'slicepos'};
        if (!defined($pos_to_image{$cur_slicepos})) {
            $pos_to_image{$cur_slicepos} = $cur_image;
            push(@positions, $cur_slicepos);
        }
    }
    @positions = sort(numeric_order @positions);

    # Get smallest step size and get nslices and ordered list of images
    local(@image_list, $slicestep, $nslices, $slicestart);
    if (scalar(@positions) > 1) {
        $slicestep = $positions[1] - $positions[0];
        foreach $i (2..$#positions) {
            local($diff) = $positions[$i] - $positions[$i-1];
            if (($diff < $slicestep) && ($diff > 0)) {
                $diff = $slicestep;
            }
        }
        if ($slicestep <= 0) {
            $nslices = scalar(@positions);
            $slicestep = 1;
            @image_list = @pos_to_image{@positions};
        }
        else {
            local($first_image, $last_image);
            $first_image = $pos_to_image{$positions[0]};
            $last_image = $pos_to_image{$positions[$#positions]};
            $nslices = 
                int(($mincinfo{$last_image, 'slicepos'} -
                     $mincinfo{$first_image, 'slicepos'}) / $slicestep + 1.5);
            foreach $i (0..$nslices) {
                $image_list[$i] = '';
            }
            foreach $position (@positions) {
                local($slice) = 
                    int(($position - $positions[0]) / $slicestep + 0.5);
                @image_list[$slice] = $pos_to_image{$position};
            }
        }
    }
    else {
        $slicestep = 1;
        $nslices = 1;
        @image_list = @pos_to_image{@positions};
    }
    $slicestart = $positions[0];

    # Check for existence of file - create a new name if it exists
    local($filebase);
    if ($mincfile =~ /^(.*)\.mnc$/) {
        $filebase = $1;
    }
    else {
        $filebase = $mincfile;
    }
    local($version) = 1;
    while (-e $mincfile) {
        $version++;
        $mincfile = $filebase."_version$version.mnc";
        print STDERR "Minc file already exists. Trying name \"$mincfile\".\n";
    }

    # Set up rawtominc command

    # Dimension info
    local($nrows, $ncols, $orientation, $pixel_size);
    local($xstep, $ystep, $zstep);
    $pixel_size = $mincinfo{'pixel_size'};
    $nrows = $mincinfo{'height'};
    $ncols = $mincinfo{'width'};
    $xstep = $mincinfo{'colstep'};
    $ystep = $mincinfo{'rowstep'};
    $zstep = $slicestep;

    # Orientation
    local($xstart, $ystart, $zstart);
    local($orient_flag) = $mincinfo{'orientation'};
    if ($orient_flag eq 'sagittal') {    # Sagittal
        ($ystep, $zstep, $xstep) = 
            ($mincinfo{'colstep'}, $mincinfo{'rowstep'}, $slicestep);
        ($ystart, $zstart, $xstart) = (0, 0, $slicestart);
        $orientation = "-sagittal";
    }
    elsif ($orient_flag eq 'coronal') { # Coronal
        ($xstep, $zstep, $ystep) = 
            ($mincinfo{'colstep'}, $mincinfo{'rowstep'}, $slicestep);
        ($xstart, $zstart, $ystart) = (0, 0, $slicestart);
        $orientation = "-coronal";
    }
    else {                      # Transverse
        ($xstep, $ystep, $zstep) = 
            ($mincinfo{'colstep'}, $mincinfo{'rowstep'}, $slicestep);
        ($xstart, $ystart, $zstart) = (0, 0, $slicestart);
        $orientation = "-transverse";
    }

    # Optional attributes
    local(@optional_attributes) = ();
    &add_optional_attribute(*optional_attributes, 
                            "acquisition:repetition_time", $mincinfo{'tr'});
    &add_optional_attribute(*optional_attributes, 
                            "acquisition:echo_time", $mincinfo{'te',$echo});
    &add_optional_attribute(*optional_attributes,
                            "acquisition:inversion_time", $mincinfo{'ti'});
    &add_optional_attribute(*optional_attributes, 
                            "acquisition:flip_angle", $mincinfo{'mr_flip'});
    &add_optional_attribute(*optional_attributes, 
                            "acquisition:scanning_sequence", 
                            $mincinfo{'scan_seq'});
    &add_optional_attribute(*optional_attributes, 
                            "study:study_id", $mincinfo{'exam'});
    &add_optional_attribute(*optional_attributes, 
                            "study:acquisition_id", $mincinfo{'series'});
    &add_optional_attribute(*optional_attributes, 
                            "study:start_time", $mincinfo{'start_time'});
    &add_optional_attribute(*optional_attributes, 
                            "study:institution", $mincinfo{'institution'});
    &add_optional_attribute(*optional_attributes, 
                            "patient:birthdate", 
                            $mincinfo{'patient_birthdate'});
    &add_optional_attribute(*optional_attributes, 
                            "patient:age", $mincinfo{'patient_age'});
    &add_optional_attribute(*optional_attributes, 
                            "patient:sex", $mincinfo{'patient_sex'});
    &add_optional_attribute(*optional_attributes, 
                            "patient:identification", $mincinfo{'patient_id'});

    # GE specific stuff
    local(@ge_specific_attributes);
    @ge_specific_attributes = ();
    &add_optional_attribute(*ge_specific_attributes,
                            "ge_mrimage:pseq", $mincinfo{'ge_pseq'});
    &add_optional_attribute(*ge_specific_attributes,
                            "ge_mrimage:pseqmode", $mincinfo{'ge_pseqmode'});

    # Run rawtominc with appropriate arguments
    $| = 1;
    open(MINC, "|-") || exec
        ("rawtominc", $mincfile, $nslices, $nrows, $ncols, "-noclobber",
         "-scan_range", "-short", "-range", "0", "4095", $orientation,
         "-xstep", $xstep, "-ystep", $ystep, "-zstep", $zstep,
         "-xstart", $xstart, "-ystart", $ystart, "-zstart", $zstart,
         "-mri", 
         "-attribute", "patient:full_name=".$mincinfo{'patient_name'},
         @optional_attributes,
         @ge_specific_attributes,
         );

    # Get keys for machine specific file info
    local(@specific_keys, %specific_keys, $key);
    undef(%specific_keys);
    foreach $key (keys(%mincinfo)) {
        $key =~ /$;specific$;([^$;]*)$/;
        $specific_keys{$1} = '';
    }
    @specific_keys = keys(%specific_keys);

    # Loop through slices
    local($cur_file, %specific_file_info, $cur_image);
    foreach $slice (0..$nslices-1) {

        # Get image number
        $cur_image = $image_list[$slice];

        # Get file name
        $cur_file = $file_list{$cur_image};

        # Print log message
        if (length($cur_file) > 0) {
            print STDERR 
                "Getting data from file \"$cur_file\" for slice $slice.\n";
        }
        else {
            print STDERR "Using blank image for slice $slice.\n";
        }

        # Set up variables for getting the image
        $image_data_len = $nrows * $ncols * $pixel_size;

        # Read the image (if not defined, create a blank image)
        if (length($cur_file) <= 0) {
            $image_data = pack("x$image_data_len",());
        }
        else {
            # Get machine specific file info 
            undef(%specific_file_info);
            foreach $key (@specific_keys) {
                $specific_file_info{$key} = 
                    $mincinfo{$cur_image, 'specific', $key};
            }

            local($image_cmd) = 
                &get_image_cmd($cur_file, *specific_file_info);
            open(GEDAT, $image_cmd . " |");
            read(GEDAT, $image_data, $image_data_len);
            close(GEDAT);
            if ($? != 0) {
                &cleanup_and_die("Error or signal while reading image.\n", $?);
            }
            if (length($image_data) != $image_data_len) {
                close(MINC);
                warn "Error reading image from $cur_file ".
                    "while creating minc file \"$mincfile\"";
                return;
            }
        }

        # Write out the image
        print MINC $image_data;
    }

    # Close the minc file
    close(MINC);
    if ($? != 0) {
        &cleanup_and_die("Error or signal while writing image.\n", $?);
    }

}

# Subroutine to do all the work - loops through files collecting info,
# then calls routine to create minc file.
# Because this was the main program, it uses global variables
sub mri_to_minc {

    $| = 1;
    $outputdir = shift;
    if (!defined($outputdir)) {
        die "Usage: $0 <outputdir> [/dev/<tape device> | <file1> <file2> ...]";
    }

    # Get arguments
    if (!defined($_[0])) {
        $tapedrive = "/dev/nrtape";
        @input_list = ();
    }
    elsif ($_[0] =~ m+^/dev/+) {
        $tapedrive = shift;
        @input_list = ();
        if (scalar(@_) > 0) {
            die "Do not specify tapedrive and input file list, stopped";
        }
    }
    else {
        $tapedrive = '';
        @input_list = @_;
    }

    # Should we delete the files?
    $delete_files = (length($tapedrive) > 0);

    # Create a temporary directory
    &create_tmpdir("mri_to_minc_$$");

    # Rewind and initialize the tape
    if (length($tapedrive) > 0) {
        &initialize_tape_drive($tapedrive);
    }

    # Loop through files on tape
    $keep_looping = 1;
    while ($keep_looping) {

        # Get next file
        $nextfile = &read_next_file($tapedrive, *input_list);
        if ($nextfile eq "") {
            $keep_looping = 0;
        }

        # Read in headers
        if ($keep_looping) {
            undef(%file_info, %specific_file_info);
            if (&read_file_info($nextfile, *file_info, *specific_file_info)) {
                warn "Error reading file \"$nextfile\". Skipping to next.";
                next;
            }

            # Get interesting values
            $cur_numechos = $file_info{'numechos'};
            $cur_exam = $file_info{'exam'};
            $cur_series = $file_info{'series'};
            $cur_echo = $file_info{'echo'};
            $cur_image = $file_info{'image'} * $cur_numechos + $cur_echo;
            $cur_width = $file_info{'width'};
            $cur_height = $file_info{'height'};
            $cur_slicepos = $file_info{'slicepos'};
        }

        # Check if number exam or series has changed or if this is the 
        # last file
        if ((scalar(keys(%file_list)) > 0) &&
            (!$keep_looping || 
             ($mincinfo{'exam'} ne $cur_exam) || 
             ($mincinfo{'series'} != $cur_series))) {

            # Loop through echos
            @echos = sort(numeric_order keys(%echo_list));
            if (scalar(@echos) > $mincinfo{'numechos'}) {
                $mincinfo{'numechos'} = scalar(@echos);
            }
            foreach $echo (@echos) {

                # Create minc file
                ($patient_name = $mincinfo{'patient_name'}) =~
                    tr/a-zA-Z0-9_\-/_/cs;
                $patient_name =~ tr/A-Z/a-z/;
                $mincfile = "$outputdir/".$patient_name."_".
                    $mincinfo{'exam'}."_".$mincinfo{'series'};
                if ($mincinfo{'numechos'} > 1) {
                    $mincfile .= "_e$echo";
                }
                $mincfile .= "_mri.mnc";
                print STDERR "Creating minc file \"$mincfile\"\n";
                &create_mincfile($mincfile, *file_list, *mincinfo, 
                                 *image_list, $echo);
            }

            # Delete files (if needed) and reset variables
            if ($delete_files) {
                &remove_file(values(%file_list));
            }
            undef(%file_list, %echo_list, %image_list, %mincinfo);
        }

        # Break out here if stopping loop
        if (!$keep_looping) {next;}

        # If first file, then save appropriate info
        if (scalar(keys(%file_list)) <= 0) {
            $mincinfo{'exam'} = $cur_exam;
            $mincinfo{'series'} = $cur_series;
            $mincinfo{'width'} = $cur_width;
            $mincinfo{'height'} = $cur_height;
            $mincinfo{'pixel_size'} = $file_info{'pixel_size'};
            $mincinfo{'patient_name'} = $file_info{'patient_name'};
            $mincinfo{'orientation'} = $file_info{'orientation'};
            $mincinfo{'colstep'} = $file_info{'colstep'};
            $mincinfo{'rowstep'} = $file_info{'rowstep'};
            $mincinfo{'tr'} = $file_info{'tr'};
            $mincinfo{'ti'} = $file_info{'ti'};
            $mincinfo{'mr_flip'} = $file_info{'mr_flip'};
            $mincinfo{'scan_seq'} = $file_info{'scan_seq'};
            $mincinfo{'ge_pseq'} = $file_info{'ge_pseq'};
            $mincinfo{'ge_pseqmode'} = $file_info{'ge_pseqmode'};
            $mincinfo{'start_time'} = $file_info{'start_time'};
            $mincinfo{'institution'} = $file_info{'institution'};
            $mincinfo{'patient_birthdate'} = $file_info{'patient_birthdate'};
            $mincinfo{'patient_age'} = $file_info{'patient_age'};
            $mincinfo{'patient_sex'} = $file_info{'patient_sex'};
            $mincinfo{'patient_id'} = $file_info{'patient_id'};
        }
        else {
            if (($cur_width != $mincinfo{'width'}) || # 
                ($cur_height != $mincinfo{'height'})) {
                warn "Width or height do not match first file ".
                    "for file $nextfile. Skipping to next.";
                next;
            }
        }

        # Save echo number
        if (!defined($mincinfo{'te', $cur_echo})) {
            $mincinfo{'te', $cur_echo} = $file_info{'te'};
        }

        # Save info for all files
        foreach $key (keys(%specific_file_info)) {
            $mincinfo{$cur_image, 'specific', $key} =
                $specific_file_info{$key};
        }
        $mincinfo{$cur_image, 'slicepos'} = $cur_slicepos;

        # Keep track of files, image numbers and echo numbers
        $file_list{$cur_image} = $nextfile;
        $echo_list{$cur_echo} = '';
        if (defined($image_list{$cur_echo})) {$image_list{$cur_echo} .= $; ;}
        $image_list{$cur_echo} .= $cur_image;

    }

    # Rewind the tape
    if (length($tapedrive) > 0) {
        &execute("mt -t $tapedrive rewind");
    }

    &cleanup_and_die();
}

