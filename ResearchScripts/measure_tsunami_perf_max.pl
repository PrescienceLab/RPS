#! /usr/bin/env perl

# This script performs full out tests, running each utility as fast as possible.
#
# Arguments:
#
#  inputfile -> A file consisting of time-series samples.  This should be quite
#               a big number of samples (~1million)
#  blocksize -> The blocksize used for block tests
#  numtests  -> The number of tests to run, which will be averaged finally.

$usage = "measure_tsunami_perf_max.pl inputfile blocksize numtests\n";

$#ARGV==2 or die $usage;

$file = shift;;
$blocksize = shift;
$numtests = shift;

# Parameters for tests
$NUM_WAVELET_TYPES = 10;
$MAX_STAGES = 20;
$FLAT = "flat";
$OUTDIR = "/home/jskitz/RPS-development/Wavelets/bin/I386/LINUX/outfiles";
@wavelet_types = ("DAUB2", "DAUB4", "DAUB6", "DAUB8", "DAUB10", "DAUB12", "DAUB14", "DAUB16", "DAUB18", "DAUB20");
@stage_nums = ("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20");

# Dummy argument when not needed
$NOT_USED = 1;

# STREAMING TESTS, 120 outputs
# ----------------------------

# Streaming scalability test as levels added, sample, forward, 20 outputs
# -----------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.sample.DAUB10.t.max.out\n";
  system "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.sample.DAUB10.t.max.out";
}

# Streaming scalability as wavelet type order increases, sample, forward, 10 outputs
# ----------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.sample.10-stages.t.max.out\n";
  system "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.sample.10-stages.t.max.out";
}

# Streaming scalability test as levels added, block, forward, 20 outputs
# ----------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.block.DAUB10.t.max.out\n";
  system "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.block.DAUB10.t.max.out";
}

# Streaming scalability as wavelet type order increases, block, forward, 10 outputs
# ---------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.block.10-stages.t.max.out\n";
  system "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.block.10-stages.t.max.out";
}

# Streaming scalability test as levels added, sample, delay + reverse, 20 outputs
# -------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.sample.DAUB10.t.max.out\n";
  system "samplerate_perf_srwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.sample.DAUB10.t.max.out";
}

# Streaming scalability as wavelet type order increases, sample, delay + reverse, 10 outputs
# ------------------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.sample.10-stages.t.max.out\n";
  system "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.sample.10-stages.t.max.out";
}

# Streaming scalability test as levels added, block, delay + reverse, 20 outputs
# ------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.block.DAUB10.t.max.out\n";
  system "samplerate_perf_srwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.block.DAUB10.t.max.out";
}

# Streaming scalability as wavelet type order increases, block, delay + reverse, 10 outputs
# -----------------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.block.10-stages.t.max.out\n";
  system "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize  0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.block.10-stages.t.max.out";
}


# DISCRETE TESTS, 40 outputs
# --------------------------

# Discrete forward scalability test as blocksize increases -> levels increase, 10 outputs
# ---------------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming dft operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "samplerate_perf_dft $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.dft.DAUB10.t.max.out\n";
  system "samplerate_perf_dft $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.dft.DAUB10.t.max.out";
}


# Discrete forward scalability test as wavelet type increases order, blocksize = 1024, 10 outputs
# -----------------------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_dft $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.dft.$blocksize.t.max.out\n";
  system "samplerate_perf_dft $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.dft.$blocksize.t.max.out";
}

# Discrete reverse scalability test as blocksize increases -> levels increase, 10 outputs
# ---------------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming drt operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "samplerate_perf_drt $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.drt.DAUB10.t.max.out\n";
  system "samplerate_perf_drt $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.drt.DAUB10.t.max.out";
}


# Discrete reverse scalability test as wavelet type increases order, blocksize = $blocksize, 10 outputs
# -----------------------------------------------------------------------------------------------------
print STDERR "\nScalability test for drt operation, blocksize = $blocksize, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_drt $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.drt.$blocksize.t.max.out\n";
  system "samplerate_perf_drt $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.drt.$blocksize.t.max.out";
}
