#! /usr/bin/env perl
#
# Script to produce mincmorph structuring elements
#
# Alex Zidjdenbos
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies.  The
# author makes no representations about the suitability of this software
# for any purpose.  It is provided "as is" without express or implied warranty.


use strict;
use warnings "all";

use Cwd qw/ abs_path getcwd /;
use File::Basename;
use File::Temp qw/ tempdir /;
use Getopt::Tabular;
use IO::File;
use POSIX qw/floor ceil/;

my $Clobber = 0;
my $Execute = 1;
my $Verbose = 0;
my $R       = 2;
my @R3;

my $Me = fileparse($0);

my $Usage = <<USAGE;

Usage: $Me [options] <file.kern>

USAGE

my $Help = <<HELP;

$Me generates a spherical kernel (structuring element) suitable for use with mincmorph.

HELP

&Getopt::Tabular::SetHelp($Help, $Usage);

my @ArgTable = (
    ["General options", "section"],
    ["-clobber", "boolean", 1, \$Clobber, "Overwrite existing files"],
    ["-verbose|-quiet", "boolean", 1, \$Verbose, "Be verbose"],
    ["-execute", "boolean", 1, \$Execute, "Execute commands"],

    ["Specific options", "section"],
    ["-radius", "float", 1, \$R, "Radius of spherical kernel (in voxels)", "<radius>"], 
    ["-radius3", "float", 3, \@R3, "Radii of ellipsoidal kernel (in voxels). Forces", "<x_radius> <y_radius> <z_radius>"], 
    );

my @LeftOverArgs;
&GetOptions(\@ArgTable, \@ARGV, \@LeftOverArgs) || exit 1;

die $Usage if (@LeftOverArgs != 1);

my $File = shift @LeftOverArgs;

die "$File exists; use -clobber to overwrite\n"
  if (! $Clobber && -e $File);

my $fh = new IO::File ">$File";

die "Unable to open file $File\n" 
  if (! defined $fh);

@R3 = ($R, $R, $R) if ! @R3;

my $Hdr = <<HDR;
MNI Morphology Kernel File
%
% 0 - center voxel
% Format:  offset for voxel in 5 dimensions (x,y,z,t,v) then multiplier.

Kernel_Type = Normal_Kernel;
Kernel =
%    x    y    z    t    v     coeff
% -----------------------------------
HDR

print $fh $Hdr;

for (my $z = floor(-$R3[2]); $z <= ceil($R3[2]); $z++) {
    my $rz = ($R3[2]) ? ($z/$R3[2])**2 : 0;

    for (my $y = floor(-$R3[1]); $y <= ceil($R3[1]); $y++) {
	my $ry = ($R3[1]) ? ($y/$R3[1])**2 : 0;

        for (my $x = floor(-$R3[0]); $x <= ceil($R3[0]); $x++) {
	    my $rx = ($R3[0]) ? ($x/$R3[0])**2 : 0;
	    my $r = $rx + $ry + $rz;
	    printf $fh "\n  %4.1f %4.1f %4.1f %4.1f %4.1f    %4.1f", $x, $y, $z, 0, 0, 1.0
		if ($r && $r <= 1);
	}
    }
}
print $fh ";\n";

$fh->close();
