#!/usr/bin/env perl

# This script performs full out tests, running each utility as fast as possible.
#
# Arguments:
#
#  inputfile     -> A file consisting of time-series samples.  This should be quite
#                   a big number of samples (~1million)
#  initsize      -> Data size that should be transformed into sfwt and dft
#  initblocksize -> The initial blocksize used for discrete tests

$usage = "measure_tsunami_system_perf.pl inputfile initsize initblocksize hz\n";

$#ARGV==3 or die $usage;

$file = shift;;
$initsize = shift;
$initblocksize =shift;
$hz = shift;

$maxhz=1000;

$NOT_USED=1;
$FLAT="flat";

use FileHandle;

autoflush STDOUT 1;

$OUTDIR="/home/jskitz/RPS-development/Wavelets/bin/I386/LINUX/outfiles";

system "kill_matching.pl clean_vmstat";
system "kill_matching.pl clean_loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl load2measure";
system "kill_matching.pl loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl vmstat";

# start up the vmstat monitor
print STDERR "Start vmstat monitor\n";
system "clean_vmstat.pl > $OUTDIR/vmstat.out &";

# capture quiescent behavior
sleep(20);

# start up a load monitor
print STDERR "Start loadserver monitor\n";
system "clean_loadserver.pl 1000000 10000 > $OUTDIR/loadmonitor.out &";

# capture combined behavior
sleep(30);

if (1) {

  #-----------------------------------------------------------------------------

  #SAMPLE PROCESSING, FORWARD

  $datasize = $initsize;
  $blk = $initblocksize;

  if ($hz > 100) {
    $usec = 0;
  } else {
    $usec = int(1000000/$hz);
  }
  print STDERR "$hz\t$usec\n";
  system "echo \"perf_sfwt1 $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $blk $usec $FLAT stdout > /dev/null\"";
  system "perf_sfwt1 $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $blk $usec $FLAT stdout > /dev/null";
}

if (0) {

  #-----------------------------------------------------------------------------

  #SAMPLE PROCESSING, REVERSE

  $datasize = $initsize;
  for (;$hz<=$maxhz;$hz*=10) {
    if ($hz > 100) {
      $usec = 0;
      $datasize = 4194304;
    } else {
      $usec = int(1000000/$hz);
    }
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $usec $FLAT stdout > /dev/null\"";
    system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $usec $FLAT stdout > /dev/null";
    $datasize*=4;
  }
}

if (0) {

  #DISCRETE BLOCK PROCESSING, FORWARD

  $datasize = $initsize;
  $blk = $initblocksize;

  if ($hz > 100) {
    $usec = 0;
  } else {
    $usec = int(1000000/$hz);
  }
  print STDERR "$hz\t$usec\n";
  system "echo \"perf_dft1 $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
  system "perf_dft1 $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
}

if (0) {

  #DISCRETE BLOCK PROCESSING, REVERSE

  $datasize = $initsize;
  $blk = $initblocksize;

  if ($hz > 100) {
    $usec = 0;
  } else {
    $usec = int(1000000/$hz);
  }
  print STDERR "$hz\t$usec\n";
  system "echo \"perf_drt1 $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
  system "perf_drt1 $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
}

sleep(240);


# Performance tests
if (0) {

  #-----------------------------------------------------------------------------

  #SAMPLE PROCESSING, FORWARD THEN REVERSE

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of sfwt, sample\n";
  $datasize = $initsize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $NOT_USED $usec $FLAT stdout > /dev/null\"";
    system "perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $NOT_USED $usec $FLAT stdout > /dev/null";
    $datasize*=2;
  }
  system "echo \"perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $FLAT stdout > /dev/null\"";
  system "perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM SAMPLE $NOT_USED 0 $FLAT stdout > /dev/null";

  sleep(10);

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of srwt, sample\n";
  $datasize = $initsize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $usec $FLAT stdout > /dev/null\"";
    system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED $usec $FLAT stdout > /dev/null";
    $datasize*=2;
  }
  system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED 0 $FLAT stdout > /dev/null\"";
  system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $NOT_USED $NOT_USED 0 $FLAT stdout > /dev/null";

  sleep(10);

  #-----------------------------------------------------------------------------

  #BLOCK PROCESSING, FORWARD THEN REVERSE

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of sfwt, block\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    $usec*=$blk;
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }
  system "echo \"perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk 0 $FLAT stdout > /dev/null\"";
  system "perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk 0 $FLAT stdout > /dev/null";

  sleep(10);

  # now use my utilities to sweep the sleeprates (lat_perf_TTYPE)
  print STDERR "Sweep Hz for performance of srwt, block\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  $numblocks = $datasize / $blk;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    $usec*=$blk;
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks $usec $FLAT stdout > /dev/null\"";
    system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }
  system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks 0 $FLAT stdout > /dev/null\"";
  system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks 0 $FLAT stdout > /dev/null";

  sleep(10);

  #-----------------------------------------------------------------------------

  #DISCRETE BLOCK PROCESSING, FORWARD THEN REVERSE

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of dft\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    $usec*=$blk;
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }
  system "echo \"perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk 0 $FLAT stdout > /dev/null\"";
  system "perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk 0 $FLAT stdout > /dev/null";

  sleep(10);

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of drt\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    $usec*=$blk;
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }
  system "echo \"perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk 0 $FLAT stdout > /dev/null\"";
  system "perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk 0 $FLAT stdout > /dev/null";

  #-----------------------------------------------------------------------------

}

# Latency tests
if (0) {

  # now use my utility to sweep the sleeprates (lat_perf_TTYPE)
  print STDERR "Sweep Hz for latency\n";
  system "echo \"blocksize=1024 10 stages 8192 samples DAUB10\" > $OUTDIR/latency.out";
#  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz); 
    print STDERR "$hz\t$usec\n";
    system "echo \"$hz Hertz\" >> $OUTDIR/latency.out";
    system "lat_perf_sfwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.sfwt.sample.DAUB10.t.$usec.out";
    system "lat_perf_sfwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.sfwt.sample.DAUB10.t.$usec.out";
    system "lat_perf_srwt $file DAUB10 10 TRANSFORM SAMPLE $NOT_USED $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.srwt.sample.DAUB10.t.max.out";
    system "lat_perf_srwt $file DAUB10 10 TRANSFORM BLOCK $blocksize $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.srwt.sample.DAUB10.t.max.out";
    system "lat_perf_dft $file DAUB10 TRANSFORM $blocksize $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.dft.DAUB10.t.max.out";
    system "lat_perf_drt $file DAUB10 TRANSFORM $blocksize $usec $numtests $FLAT stdout >> $OUTDIR/perf.lat.drt.DAUB10.t.max.out";
#  }
}

system "kill_matching.pl clean_vmstat";
system "kill_matching.pl clean_loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl load2measure";
system "kill_matching.pl loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl vmstat";
