#!/usr/bin/env perl

# This script generates test data for performance testing.  It will generate data from
# the forward transform for use in the reverse transform.  It will also generate data for
# the discrete forward and reverse transform data.
#
# Arguments:
#
#  inputfile     -> Input file prefix (i.e. axp7_data)
#  size          -> Data size that should be transformed into sfwt and dft
#  initblocksize -> The initial blocksize used for discrete tests


$usage = "generate_tsunami_scalability_data.pl inputfile size initblocksize\n";

$#ARGV==2 or die $usage;

$file = shift;
$initsize = shift;
$initblocksize = shift;

# defines
@wt = ("DAUB2", "DAUB4", "DAUB6", "DAUB8", "DAUB10", "DAUB12", "DAUB14", "DAUB16", "DAUB18", "DAUB20");
@stage_nums = ("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20");
$MAX_STAGES = 20;

# Generate the data for the static reverse tests based on input file data
$datasize = $initsize;
for ($i=1; $i<=$MAX_STAGES; $i++) {
  foreach $type (@wt) {
    system "echo \"sample_static_sfwt $file.$datasize.in $type $i TRANSFORM flat $file.$datasize.sfwt.$type.$i.t.out\"";
    system "sample_static_sfwt $file.$datasize.in $type $i TRANSFORM flat $file.$datasize.sfwt.$type.$i.t.out";
  }
}

# Generate the data for the discrete reverse tests
$datasize = $initsize;
for ($blk=$initblocksize; $blk<=$datasize; $blk*=2) {
  foreach $type (@wt) {
    system "echo \"discrete_forward_transform_blocks $file.$datasize.in $type TRANSFORM $blk flat $file.$datasize.dft.$type.$blk.t.out\"";
    system "discrete_forward_transform_blocks $file.$datasize.in $type TRANSFORM $blk flat $file.$datasize.dft.$type.$blk.t.out";
  }
}
