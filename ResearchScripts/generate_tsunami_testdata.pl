#!/usr/bin/env perl

# This script generates test data for performance testing.  It will generate data from
# the forward transform for use in the reverse transform.  It will also generate data for
# the discrete forward and reverse transform data.
#
# Arguments:
#
#  inputfile     -> Input file prefix (i.e. axp7_data)
#  initsize      -> Data size that should be transformed into sfwt and dft
#  initblocksize -> The initial blocksize used for discrete tests


$usage = "generate_tsunami_testdata.pl inputfile initsize initblocksize\n";

$#ARGV==2 or die $usage;

$file = shift;
$initsize = shift;
$initblocksize = shift;

# defines
$maxhz=2048;

# Generate the data for the static reverse tests based on input file data
$datasize = $initsize;
for ($hz=1;$hz<=$maxhz;$hz*=2) {
  system "echo \"sample_static_sfwt $file.$datasize.in DAUB10 10 TRANSFORM flat $file.$datasize.sfwt.DAUB10.10.t.out\"";
  system "sample_static_sfwt $file.$datasize.in DAUB10 10 TRANSFORM flat $file.$datasize.sfwt.DAUB10.10.t.out";
  $datasize*=2;
}

# Generate the data for the discrete reverse tests
$datasize = $initsize;
$blk = $initblocksize;
for ($hz=1;$hz<=$maxhz;$hz*=2) {
  system "echo \"discrete_forward_transform_blocks $file.$datasize.in DAUB10 TRANSFORM $blk flat $file.$datasize.dft.DAUB10.$blk.t.out\"";
  system "discrete_forward_transform_blocks $file.$datasize.in DAUB10 TRANSFORM $blk flat $file.$datasize.dft.DAUB10.$blk.t.out";
  $datasize*=2;
  $blk*=2;
}
