#!/usr/bin/env perl

# This script performs full out tests, running each utility as fast as possible.
#
# Arguments:
#
#  inputfile     -> A file consisting of time-series samples.  This should be quite
#                   a big number of samples (~1million)
#  initsize      -> Data size that should be transformed into sfwt and dft
#  initblocksize -> The initial blocksize used for discrete tests

$usage = "measure_tsunami_system_perf.pl inputfile initsize initblocksize\n";

$#ARGV==2 or die $usage;

$file = shift;;
$initsize = shift;
$initblocksize =shift;

$maxhz=1024;

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
sleep(120);

# start up a load monitor
print STDERR "Start loadserver monitor\n";
system "clean_loadserver.pl 1000000 10000 > $OUTDIR/loadmonitor.out &";

# capture combined behavior
sleep(120);

# Performance tests
if (1) {

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

  sleep(120);

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of sfwt, block\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_sfwt $file.$datasize.in DAUB10 10 TRANSFORM BLOCK $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }

  sleep(120);

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

  sleep(120);

  # now use my utilities to sweep the sleeprates (lat_perf_TTYPE)
  print STDERR "Sweep Hz for performance of srwt, block\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  $numblocks = $datasize / $blk;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks $usec $FLAT stdout > /dev/null\"";
    system "perf_srwt $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM BLOCK $blk $numblocks $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }

  sleep(120);

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of dft\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_dft $file.$datasize.in DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }

  sleep(120);

  # now use my utilities to sweep the sleeprates
  print STDERR "Sweep Hz for performance of drt\n";
  $datasize = $initsize;
  $blk = $initblocksize;
  for ($hz=1;$hz<=$maxhz;$hz*=2) {
    $usec = int(1000000/$hz);
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null\"";
    system "perf_drt $file.$datasize.dft.DAUB10.$blk.t.out DAUB10 TRANSFORM $blk $usec $FLAT stdout > /dev/null";
    $datasize*=2;
    $blk*=2;
  }
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

sleep(60);

system "kill_matching.pl clean_vmstat";
system "kill_matching.pl clean_loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl load2measure";
system "kill_matching.pl loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl vmstat";
