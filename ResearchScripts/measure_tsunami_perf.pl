#! /usr/bin/env perl

$usage = "measure_tsunami_perf.pl inputfile init-sleeprate_us sr-multiplier num-srs blocksize numtests\n";

$#ARGV==5 or die $usage;

$file = shift;;
$sleeprate = shift;
$multiplier = shift;
$numsrtests = shift;
$blocksize = shift;
$numtests = shift;

# Parameters for tests
$NUM_WAVELET_TYPES = 10;
$MAX_STAGES = 20;
$FLAT = "flat";
@wavelet_types = ("DAUB2", "DAUB4", "DAUB6", "DAUB8", "DAUB10", "DAUB12", "DAUB14", "DAUB16", "DAUB18", "DAUB20");
@stage_nums = ("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20");

# Dummy argument when not needed
$NOT_USED = 1;

# STREAMING TESTS
# ---------------

# Max samplerate test, sample, forward
# ------------------------------------
print STDERR "Maximum sample rate test for streaming forward sample operation.\n";

# Determine maximum rate
print STDERR "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.sr.sfwt.sample.DAUB10.10.t.0.out\n";
system "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.sr.sfwt.sample.DAUB10.10.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $nxt_srate $numtests $flat perf.sr.sfwt.sample.DAUB10.10.t.$nxt_srate.out\n";
  system "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $nxt_srate $numtests $flat perf.sr.sfwt.sample.DAUB10.10.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Max samplerate test, block, forward
# -----------------------------------
print STDERR "\nMaximum sample rate test for streaming forward block operation, blocksize = $blocksize.\n";

# Determine maximum rate
print STDERR "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.sr.sfwt.block.DAUB10.10.t.0.out\n";
system "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.sr.sfwt.block.DAUB10.10.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $nxt_srate $numtests $flat perf.sr.sfwt.block.DAUB10.10.t.$nxt_srate.out\n";
  system "samplerate_perf_sfwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $nxt_srate $numtests $flat perf.sr.sfwt.block.DAUB10.10.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Max samplerate test, sample, delay + reverse
# --------------------------------------------
print STDERR "\nMaximum sample rate test for streaming reverse sample operation.\n";

# Determine maximum rate
print STDERR "samplerate_perf_srwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.sr.srwt.sample.DAUB10.10.t.0.out\n";
system "samplerate_perf_srwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.sr.srwt.sample.DAUB10.10.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $nxt_srate $numtests $flat perf.sr.srwt.sample.DAUB10.10.t.$nxt_srate.out\n";
  system "samplerate_perf_srwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $nxt_srate $numtests $flat perf.sr.srwt.sample.DAUB10.10.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Max samplerate test, block, delay + reverse
# -------------------------------------------
print STDERR "\nMaximum sample rate test for streaming reverse block operation, blocksize = $blocksize.\n";

# Determine maximum rate
print STDERR "samplerate_perf_srwt $file DAUB10 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.sr.srwt.block.DAUB10.10.t.0.out\n";
system "samplerate_perf_srwt $file DAUB10 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.sr.srwt.block.DAUB10.10.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $nxt_srate $numtests $flat perf.sr.srwt.block.DAUB10.10.t.$nxt_srate.out\n";
  system "samplerate_perf_srwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $nxt_srate $numtests $flat perf.sr.srwt.block.DAUB10.10.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Streaming scalability test as levels added, sample, forward
# -----------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalst.sfwt.sample.DAUB10.$stage_nums[$i]-stages.t.0.out\n";
  system "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalst.sfwt.sample.DAUB10.$stage_nums[$i]-stages.t.0.out";
}

# Streaming scalability as wavelet type order increases, sample, forward
# ----------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalwt.sfwt.sample.$wavelet_types[$i].10.t.0.out\n";
  system "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalwt.sfwt.sample.$wavelet_types[$i].10.t.0.out";
}

# Streaming scalability test as levels added, block, forward
# -----------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalst.sfwt.block.DAUB10.$stage_nums[$i]-stages.t.0.out\n";
  system "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalst.sfwt.block.DAUB10.$stage_nums[$i]-stages.t.0.out";
}

# Streaming scalability as wavelet type order increases, block, forward
# ----------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalwt.sfwt.block.$wavelet_types[$i].10.t.0.out\n";
  system "samplerate_perf_sfwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalwt.sfwt.block.$wavelet_types[$i].10.t.0.out";
}

# Streaming scalability test as levels added, sample, delay + reverse
# -------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalst.srwt.sample.DAUB10.$stage_nums[$i]-stages.t.0.out\n";
  system "samplerate_perf_srwt $file DAUB10 $i TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalst.srwt.sample.DAUB10.$stage_nums[$i]-stages.t.0.out";
}

# Streaming scalability as wavelet type order increases, sample, delay + reverse
# ------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse sample operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalwt.srwt.sample.$wavelet_types[$i].10.t.0.out\n";
  system "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM SAMPLE $NOT_USED 0 $numtests $flat perf.scalwt.srwt.sample.$wavelet_types[$i].10.t.0.out";
}

# Streaming scalability test as levels added, block, delay + reverse
# ------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, wavelet type=DAUB10, levels sweeped.\n";
for ($i=1; $i<=$MAX_STAGES; $i++) {
  print STDERR "samplerate_perf_srwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalst.srwt.block.DAUB10.$stage_nums[$i]-stages.t.0.out\n";
  system "samplerate_perf_srwt $file DAUB10 $i TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalst.srwt.block.DAUB10.$stage_nums[$i]-stages.t.0.out";
}

# Streaming scalability as wavelet type order increases, block, delay + reverse
# -----------------------------------------------------------------------------
print STDERR "\nScalability test for streaming reverse block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize 0 $numtests $flat perf.scalwt.srwt.block.$wavelet_types[$i].10.t.0.out\n";
  system "samplerate_perf_srwt $file $wavelet_types[$i] 10 TRANSFORM BLOCK $blocksize  0 $numtests $flat perf.scalwt.srwt.block.$wavelet_types[$i].10.t.0.out";
}


# DISCRETE TESTS
# ---------------

# Max sample rate test, forward transform
# ---------------------------------------
print STDERR "\nMaximum sample rate test for discrete forward transform operation.\n";

# Determine maximum rate
print STDERR "samplerate_perf_dft $file DAUB10 TRANSFORM $blocksize 0 $numtests $flat perf.sr.dft.DAUB10.$blocksize.t.0.out\n";
system "samplerate_perf_dft $file DAUB10 10 TRANSFORM $blocksize 0 $numtests $flat perf.sr.dft.DAUB10.$blocksize.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_dft $file DAUB10 TRANSFORM $blocksize $nxt_srate $numtests $flat perf.sr.dft.DAUB10.$blocksize.t.$nxt_srate.out\n";
  system "samplerate_perf_dft $file DAUB10 TRANSFORM $blocksize $nxt_srate $numtests $flat perf.sr.dft.DAUB10.$blocksize.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Max sample rate test, reverse transform
# ---------------------------------------
print STDERR "\nMaximum sample rate test for discrete reverse transform operation.\n";

# Determine maximum rate
print STDERR "samplerate_perf_drt $file DAUB10 TRANSFORM $blocksize 0 $numtests $flat perf.sr.drt.DAUB10.$blocksize.t.0.out\n";
system "samplerate_perf_drt $file DAUB10 10 TRANSFORM $blocksize 0 $numtests $flat perf.sr.drt.DAUB10.$blocksize.t.0.out";

$nxt_srate = $sleeprate;
for ($i=1; $i<$numsrtests; $i++) {
  print STDERR "samplerate_perf_drt $file DAUB10 TRANSFORM $blocksize $nxt_srate $numtests $flat perf.sr.drt.DAUB10.$blocksize.t.$nxt_srate.out\n";
  system "samplerate_perf_drt $file DAUB10 TRANSFORM $blocksize $nxt_srate $numtests $flat perf.sr.drt.DAUB10.$blocksize.t.$nxt_srate.out";
  $nxt_srate *= $multiplier;
}

# Discrete forward scalability test as blocksize increases -> levels increase
# ---------------------------------------------------------------------------
print STDERR "\nScalability test for streaming dft operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "samplerate_perf_dft $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $flat perf.scalbs.dft.DAUB10.$nxt_blksize.t.0.out\n";
  system "samplerate_perf_sfwt $file DAUB10 $i TRANSFORM $nxt_blksize 0 $numtests $flat perf.scalbs.dft.DAUB10.$nxt_blksize.t.0.out";
}


# Discrete forward scalability test as wavelet type increases order, blocksize = 1024
# -----------------------------------------------------------------------------------
print STDERR "\nScalability test for streaming forward block operation, stages=10, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_dft $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $flat perf.scalwt.dft.$wavelet_types[$i].$blksize.t.0.out\n";
  system "samplerate_perf_dft $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $flat perf.scalwt.dft.$wavelet_types[$i].$blksize.t.0.out";
}

# Discrete reverse scalability test as blocksize increases -> levels increase
# ---------------------------------------------------------------------------
print STDERR "\nScalability test for streaming drt operation, wavelet type=DAUB10, blocksize sweeped 2...1024.\n";
for ($nxt_blksize=2; $nxt_blksize<=1024; $nxt_blksize *= 2) {
  print STDERR "samplerate_perf_drt $file DAUB10 TRANSFORM $nxt_blksize 0 $numtests $flat perf.scalbs.drt.DAUB10.$nxt_blksize.t.0.out\n";
  system "samplerate_perf_drt $file DAUB10 $i TRANSFORM $nxt_blksize 0 $numtests $flat perf.scalbs.drt.DAUB10.$nxt_blksize.t.0.out";
}


# Discrete reverse scalability test as wavelet type increases order, blocksize = $blocksize
# -----------------------------------------------------------------------------------------
print STDERR "\nScalability test for drt operation, blocksize = $blocksize, wavelet type sweeped.\n";
for ($i=0; $i<$NUM_WAVELET_TYPES; $i++) {
  print STDERR "samplerate_perf_drt $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $flat perf.scalwt.dft.$wavelet_types[$i].$blksize.t.0.out\n";
  system "samplerate_perf_drt $file $wavelet_types[$i] TRANSFORM $blocksize 0 $numtests $flat perf.scalwt.dft.$wavelet_types[$i].$blksize.t.0.out";
}
