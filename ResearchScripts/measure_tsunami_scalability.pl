#! /usr/bin/env perl

# This script performs full out tests, running each utility as fast as possible.
# It tests the scalability of each of the processing units as stages are added or
# as wavelet types are changed.  It also looks at the scalability of sample vs.
# block processing.
#
# Arguments:
#
#  inputfile  -> A file consisting of time-series samples.  This should be quite
#                a big number of samples (~1million)
#  numsamples -> The number of samples in the input file.
#  blocksize  -> The blocksize used for block tests
#  numtests   -> The number of tests to run, which will be averaged finally.

$usage = "measure_tsunami_scalability.pl inputfile numsamples blocksize numtests\n";

$#ARGV==3 or die $usage;

$file = shift;
$numsamps = shift;
$blocksize = shift;
$numtests = shift;

$numblocks = $numsamps / $blocksize;

# Parameters for tests
$NUM_WAVELET_TYPES = 10;
$MAX_STAGES = 20;
$FLAT = "flat";
$OUTDIR = "/home/jskitz/RPS-development/Wavelets/bin/I386/LINUX/outfiles";
@wt = ("DAUB2", "DAUB4", "DAUB6", "DAUB8", "DAUB10", "DAUB12", "DAUB14", "DAUB16", "DAUB18", "DAUB20");
@stage_nums = ("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20");

# Dummy argument when not needed
$NOT_USED = 1;

# STREAMING TESTS
# ---------------

# Streaming scalability test as levels added, sample, forward
# -----------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "scal_perf_sfwt $file.in DAUB10 $i TRANSFORM SAMPLE $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.sample.DAUB10.t.out\n";
  system "scal_perf_sfwt $file.in DAUB10 $i TRANSFORM SAMPLE $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.sample.DAUB10.t.out";
}

# Streaming scalability as wavelet type order increases, sample, forward
# ----------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_sfwt $file.in $wt[$i] 10 TRANSFORM SAMPLE $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.sample.10-stages.t.out\n";
  system "scal_perf_sfwt $file.in $wt[$i] 10 TRANSFORM SAMPLE $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.sample.10-stages.t.out";
}

# Streaming scalability test as levels added, block, forward
# ----------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "scal_perf_sfwt $file.in DAUB10 $i TRANSFORM BLOCK $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.block.DAUB10.t.out\n";
  system "scal_perf_sfwt $file.in DAUB10 $i TRANSFORM BLOCK $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalst.sfwt.block.DAUB10.t.out";
}

# Streaming scalability as wavelet type order increases, block, forward
# ---------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_sfwt $file.in $wt[$i] 10 TRANSFORM BLOCK $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.block.10-stages.t.out\n";
  system "scal_perf_sfwt $file.in $wt[$i] 10 TRANSFORM BLOCK $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.sfwt.block.10-stages.t.out";
}

# Streaming scalability test as levels added, sample, delay + reverse
# -------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "scal_perf_srwt $file.sfwt.DAUB10.$i.t.out DAUB10 $i TRANSFORM SAMPLE $NOT_USED $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.sample.DAUB10.t.out\n";
  system "scal_perf_srwt $file.sfwt.DAUB10.$i.t.out DAUB10 $i TRANSFORM SAMPLE $NOT_USED $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.sample.DAUB10.t.out";
}

# Streaming scalability as wavelet type order increases, sample, delay + reverse
# ------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_srwt $file.sfwt.$wt[$i].10.t.out $wt[$i] 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.sample.10-stages.t.out\n";
  system "scal_perf_srwt $file.sfwt.$wt[$i].10.t.out $wt[$i] 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.sample.10-stages.t.out";
}

# Streaming scalability test as levels added, block, delay + reverse
# ------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "scal_perf_srwt $file.sfwt.DAUB10.$i.t.out DAUB10 $i TRANSFORM BLOCK $blocksize $numblocks $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.block.DAUB10.t.out\n";
  system "scal_perf_srwt $file.sfwt.DAUB10.$i.t.out DAUB10 $i TRANSFORM BLOCK $blocksize $numblocks $numtests $FLAT stdout >> $OUTDIR/perf.scalst.srwt.block.DAUB10.t.out";
}

# Streaming scalability as wavelet type order increases, block, delay + reverse
# -----------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_srwt $file.sfwt.$wt[$i].10.t.out $wt[$i] 10 TRANSFORM BLOCK $blocksize $numblocks $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.block.10-stages.t.out\n";
  system "scal_perf_srwt $file.sfwt.$wt[$i].10.t.out $wt[$i] 10 TRANSFORM BLOCK $blocksize $numblocks $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.srwt.block.10-stages.t.out";
}


# DISCRETE TESTS, 40 outputs
# --------------------------

# Discrete forward scalability test as blocksize increases -> levels increase
# ---------------------------------------------------------------------------
print STDERR "\nScalability test for streaming dft operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "scal_perf_dft $file.in DAUB10 TRANSFORM $nxt_blksize $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.dft.DAUB10.t.out\n";
  system "scal_perf_dft $file.in DAUB10 TRANSFORM $nxt_blksize $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.dft.DAUB10.t.out";
}


# Discrete forward scalability test as wavelet type increases order, blocksize = 1024
# -----------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_dft $file.in $wt[$i] TRANSFORM $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.dft.$blocksize.t.out\n";
  system "scal_perf_dft $file.in $wt[$i] TRANSFORM $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.dft.$blocksize.t.out";
}

# Discrete reverse scalability test as blocksize increases -> levels increase
# ---------------------------------------------------------------------------
print STDERR "\nScalability test for streaming drt operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "scal_perf_drt $file.dft.DAUB10.$nxt_blksize.t.out DAUB10 TRANSFORM $nxt_blksize $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.drt.DAUB10.t.out\n";
  system "scal_perf_drt $file.dft.DAUB10.$nxt_blksize.t.out DAUB10 TRANSFORM $nxt_blksize $numtests $FLAT stdout >> $OUTDIR/perf.scalbs.drt.DAUB10.t.out";
}


# Discrete reverse scalability test as wavelet type increases order, blocksize = $blocksize
# -----------------------------------------------------------------------------------------
print STDERR "\nScalability test for drt operation, blocksize = $blocksize, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "scal_perf_drt $file.dft.$wt[$i].$blocksize.t.out $wt[$i] TRANSFORM $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.drt.$blocksize.t.out\n";
  system "scal_perf_drt $file.dft.$wt[$i].$blocksize.t.out $wt[$i] TRANSFORM $blocksize $numtests $FLAT stdout >> $OUTDIR/perf.scalwt.drt.$blocksize.t.out";
}
