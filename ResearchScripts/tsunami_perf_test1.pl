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

for ($type=0; $type<4; $type++) {

  # start up the vmstat monitor
  print STDERR "Start vmstat monitor\n";
  system "clean_vmstat.pl > $OUTDIR/vmstat.out &";

  # capture quiescent behavior
  sleep(50);

  # start up a load monitor
  print STDERR "Start loadserver monitor\n";
  system "clean_loadserver.pl 1000000 10000 > $OUTDIR/loadmonitor.out &";

  # capture combined behavior
  sleep(50);

  if ($type == 0) {

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

    sleep(240);
    system "kill_matching.pl clean_vmstat";
    system "kill_matching.pl clean_loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl load2measure";
    system "kill_matching.pl loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl vmstat";

    system "cp $OUTDIR/vmstat.out $OUTDIR/vmstat.sfwt.$datasize.$blk.10.out";
    system "cp $OUTDIR/loadmonitor.out $OUTDIR/loadmonitor.sfwt.$datasize.$blk.10.out";
  }

  if ($type == 1) {

    #-----------------------------------------------------------------------------

    #SAMPLE PROCESSING, REVERSE

    $datasize = $initsize;
    $blk = $initblocksize;

    $numblks = $datasize / $blk;
    if ($hz > 100) {
      $usec = 0;
    } else {
      $usec = int(1000000/$hz);
    }
    print STDERR "$hz\t$usec\n";
    system "echo \"perf_srwt1 $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $blk $numblks $usec $FLAT stdout > /dev/null\"";
    system "perf_srwt1 $file.$datasize.sfwt.DAUB10.10.t.out DAUB10 10 TRANSFORM SAMPLE $blk $numblks $usec $FLAT stdout > /dev/null";
    sleep(240);
    system "kill_matching.pl clean_vmstat";
    system "kill_matching.pl clean_loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl load2measure";
    system "kill_matching.pl loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl vmstat";

    system "cp $OUTDIR/vmstat.out $OUTDIR/vmstat.srwt.$datasize.$blk.10.out";
    system "cp $OUTDIR/loadmonitor.out $OUTDIR/loadmonitor.srwt.$datasize.$blk.10.out";
  }

  if ($type == 2) {

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

    sleep(240);
    system "kill_matching.pl clean_vmstat";
    system "kill_matching.pl clean_loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl load2measure";
    system "kill_matching.pl loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl vmstat";

    system "cp $OUTDIR/vmstat.out $OUTDIR/vmstat.dft.$datasize.$blk.10.out";
    system "cp $OUTDIR/loadmonitor.out $OUTDIR/loadmonitor.dft.$datasize.$blk.10.out";
  }

  if ($type == 3) {

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

    sleep(240);
    system "kill_matching.pl clean_vmstat";
    system "kill_matching.pl clean_loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl load2measure";
    system "kill_matching.pl loadserver";
    system "kill_matching.pl measureclient";
    system "kill_matching.pl vmstat";

    system "cp $OUTDIR/vmstat.out $OUTDIR/vmstat.drt.$datasize.$blk.10.out";
    system "cp $OUTDIR/loadmonitor.out $OUTDIR/loadmonitor.drt.$datasize.$blk.10.out";
  }
}

system "kill_matching.pl clean_vmstat";
system "kill_matching.pl clean_loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl load2measure";
system "kill_matching.pl loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl vmstat";
