#! /usr/local/bin/perl
# Script to read in Scanditronix .DRS files (defining scx files)
# and convert them to a .h file containing mnemonics and offsets.
#
# Usage:
#    define_scx_header.perl <header_file> <drs file> [<drs file> ...]
#

# Constants
%types = (
            "B",  "byte",
            "W",  "word",
            "L",  "long",
            "F",  "float",
            "FS", "short_float",
            "S",  "string",
            "B-",  "date",
            "B:",  "time"
);
%lengths = (
            "byte", 1,
            "word", 2,
            "long", 4,
            "float", 4,
            "short_float", 2,
            "string", 1,
            "date", 1,
            "time", 1
);
@mnem_fields = 
   ("name", "type", "length", "in_file", "default", "start", "block");
@block_fields = 
   ("length", "multiplicity", "parent", "start");

# Check arguments
if ($#ARGV<1) {
   die "Usage : define_scx_header.perl <header_file> <drs file> ".
       "[<drs file> ...]\n";
}
$header_file = shift;

# Open the header file and write initial stuff
@drs_list = ();
open(hfile, ">$header_file") || die "$0 : Cannot open file $header_file\n";
@types = values(%types);
@lengths = ();
foreach $types (@types) {push(@lengths, $lengths{$types});}
print hfile '
/* Header file for scanditronix file definition */
#ifndef SCX_FILE_HEADER_DEFINITION_H
#define SCX_FILE_HEADER_DEFINITION_H

/* Mnemonic types */
enum scx_mnem_types 
   {' . join(', ', @types) . '};
static int scx_mnem_type_size[] =
   {' . join(', ', @lengths) . '};

/* Block type */
typedef struct scx_block_struct scx_block_type;
struct scx_block_struct {
   int length;
   int multiplicity;
   scx_block_type *parent;
};

/* Mnemonic type */
typedef struct scx_mnemonic_struct scx_mnemonic_type;
struct scx_mnemonic_struct {
   char *name;
   scx_mnem_types type;
   int length;
   int in_file;
   long default;
   long start;
   scx_block_type *block;
};

';

# Loop through drs files
while ($drs_file = shift) {

# Initialize the variables needed
undef(%mnemonics);
undef(%blocks);
$nmnems  = 0;
$nblocks = 0;
$blk= -1;
@block_list = ($blk);
$position = $next_position = 0;
undef $file_type;
undef $rev;
undef $ext;

#  Open the drs file
   if (!open(dfile, $drs_file)) {
      print STDERR "Unable to open drs file $drs_file\n";
      next;
   }

#  Loop through the lines of the drs file
   while (<dfile>) {

#     Look for leading lines
      if (! (defined($file_type) && defined($rev) && defined($ext))) {
         if (/^\s*TYPE\S*\s+\D*(\d+)/) {
            $file_type = $1;
            if ($file_type == 0) {die "Illegal file type : $file_type\n";}
         }
         elsif (/^\s*REV\.\S*\s+\D*(\d+)/) {
            $rev = $1;
         }
         elsif (/^\s*EXT\.\S*\s+(.*)/) {
            $ext = $1;
         }
      }

#     Look for mnemonic lines
      elsif (/^\s*[EDM]\s/) {

#        Parse the line
         ($code, $mnem, $ftype, $default, $prio, @rest) = split(' ');
         next if ($prio eq "");
         $nmnems++;

#        Get the mnemonic name
         $mnemonics{$nmnems-1, "name"} = $mnem;

#        Is the mnemonic stored in the file
         $mnemonics{$nmnems-1, "in_file"} = ($code ne "E");

#        Get data type and length (if there is a - or : on the end, and
#          the type is B, then we have a date or time)
         if ( $ftype !~ /^(\d*)(B|W|L|F|FS|S|B-|B:)$/ ) {next;}
         if ($1 ne "") {$mnemonics{$nmnems-1, "length"} = $1;}
         else {$mnemonics{$nmnems-1, "length"} = 1;}
         $mnemonics{$nmnems-1, "type"} = $types{$2};

#        Get pre-defined values
         if ($default !~ /^=(.*)$/) {next;}
         $mnemonics{$nmnems-1, "default"} = $1;

#        Get block
         $mnemonics{$nmnems-1, "block"} = $blk;

#        Increment position counters
         if ($code ne "M")
            {$position = $next_position;}
         if ($code ne "E") {
            $next_position = $position +
                     $mnemonics{$nmnems-1, "length"} * 
                     $lengths{$mnemonics{$nmnems-1, "type"}};
         }

#        Get position of first value
         if ($mnemonics{$nmnems-1, "in_file"}) 
            {$mnemonics{$nmnems-1, "start"} = $position;}
         else
            {$mnemonics{$nmnems-1, "start"} = -1;}

#        Check for file type matching first word of file
         if (($position == 0) && ($code eq 'D') && 
             ($file_type != $mnemonics{$nmnems-1, "default"})) {
            die "Type $file_type does not match first word of file (",
                  $mnemonics{$nmnems-1, "default"},")\n";
         }

      }

#     Look for reserved space lines
      elsif (/^\s*R\s/) {
         if (! /^\s*R\s+(\d+)\s+.*$/) {next;}
         $next_position += $1;
      }

#     Look for block definitions
      elsif (/^\s*([\(\)])-/) {

#        Look for block open or close
         $open = ($1 eq "(");
         if ($open) {

#           Parse block open line
            if (!/^\s*\(-+LEN=([^-]+)-+MULT=([^-]+)-+/) 
               {die "Error parsing block open\n";}
            $length = $1;
            $mult = $2;
            if (($mult !~ /^\d+$/) || ($mult < 1))
               {die "Error in block multiplicity\n";}
            if ($length !~ /^\d+$/) {$length = -1;}
            if (($length < 0) && ($mult>1))
               {die "Error in block length\n";}

#           Add new block
            $nblocks++;
            $blk = $nblocks-1;
            $blocks{$blk, "length"} = $length;
            $blocks{$blk, "multiplicity"} = $mult;
            $blocks{$blk, "parent"} = $block_list[$#block_list];
            $blocks{$blk, "start"} = $next_position;
            push(@block_list, $blk);

         }

#        Otherwise, we have a close block
         else {

#           Check for an open block
            if (($nblocks<=0) || ($#block_list<=0))
               {die "Unbalanced block bounds\n";}

#           Check the block length
            $block_length = $next_position - $blocks{$blk, "start"};
            if ($blocks{$blk,"length"} >= 0) {
               if ($blocks{$blk,"length"} != $block_length) {
                  die "Actual block length not equal to given block length\n";
               }
            }
            else {
               $blocks{$blk,"length"} = $block_length;
            }

#           Move up the pointer (only changes if multiplicity is > 1)
            $next_position += $blocks{$blk, "length"} *
                             ($blocks{$blk, "multiplicity"} - 1);

#           Pop the block
            pop(@block_list);
            $blk = $block_list[$#block_list];
         
         }

      }  # End of block

   }  #  End of read loop

#  Add drs file to list of files
   ($drs_id = "pc_".$file_type."_r_".$rev) =~ tr/\W/_/;
   if (grep(/$drs_id/, @drs_list)) {
         die "Two files with the same type and revision : $drs_id\n";
   }
   push(@drs_list, $drs_id);

   $block_array = 'drs_block_'.$drs_id;
   $mnem_array = 'drs_mnems_'.$drs_id;

   $drs_file_types{$drs_id} = $file_type;
   $drs_blocks{$drs_id} = $block_array;
   $drs_mnemonics{$drs_id} = $mnem_array;

#  Write out block table for file
   print hfile '
/* Blocks for drs file with id '.$drs_id.' */
static scx_block_type '.$block_array.'[] = {
';

   foreach $blk (0..$nblocks-1) {
      $the_block = $blocks{$blk, "parent"};
      if ($the_block < 0) {$the_block = 'NULL';}
      else {$the_block = '&('.$block_array.'['.$the_block.'])';}
      print hfile "   " . 
         $blocks{$blk, "length"} . ', ' .
         $blocks{$blk, "multiplicity"} . ', ' . $the_block . ",\n";
   }
   print hfile "   0, 0, NULL\n};\n\n";

#  Write out mnemonic table for file
   print hfile '
/* Mnemonics for drs file with id '.$drs_id.' */
static scx_mnemonic_type '.$mnem_array.'[] = {
';

   foreach $mnem (0..$nmnems-1) {

      $the_block = $mnemonics{$mnem, "block"};
      if ($the_block < 0) {$the_block = 'NULL';}
      else {$the_block = '&('.$block_array.'['.$the_block.'])';}

      $the_default = $mnemonics{$mnem, "default"};
      if ($the_default eq "") {$the_default = "0";}
      elsif ($mnemonics{$mnem, "type"} eq "string")
         {$the_default = '(long) "'.$the_default.'"';}

      print hfile '   "' . 
         $mnemonics{$mnem, "name"} . '", ' .
         $mnemonics{$mnem, "type"} . ', ' .
         $mnemonics{$mnem, "length"} . ', ' .
         $mnemonics{$mnem, "in_file"} . ', ' .
         $the_default . ', ' .
         $mnemonics{$mnem, "start"} . ', ' .
         $the_block . ",\n";
   }
   print hfile "   NULL, byte, 0, 0, 0, NULL\n};\n\n";
   
}  # End of loop over files

# Write out list of file types
print hfile '
/* List of drs files and types */

static struct {
   int file_type;
   scx_block_type *block_list;
   scx_mnemonic_type *mnemonic_list;
} scx_file_types[] = {
';

foreach $drs_id (@drs_list) {
   print hfile "   $drs_file_types{$drs_id}, " .
               "$drs_blocks{$drs_id}, " .
               "$drs_mnemonics{$drs_id},\n";
}

   print hfile '   0, NULL, NULL
};

#endif  /* SCX_FILE_HEADER_DEFINITION_H */
';
