use ExtUtils::MakeMaker;
#
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
#
# If any modules outside of the core perl distribution are required,
# these should be included as a PREREQ_PM entry in WriteMakefile below,
# as indicated in the example. This example requires the modules
# MOD1 and MOD2 to be installed, with minimal versions 1 and 5,
# respectively. If the version number is 0, any version is sufficient.

require 5.004;

WriteMakefile(
    NAME              => 'siemens_mosaic2mnc',
    VERSION_FROM      => 'siemens_mosaic2mnc',
    PREREQ_PM         => {'Getopt::Tabular' => 0.3},
    'dist'            => { COMPRESS => 'gzip', SUFFIX => 'gz' },
    'EXE_FILES'       => [ 'siemens_mosaic2mnc' ],
);
