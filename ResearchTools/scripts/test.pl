#!/usr/bin/perl -w

use FileHandle;

$#ARGV==1 or die "usage: test.pl configfile testfile\n";

$configfile=shift;
$testfile=shift;

system "bin/I386/LINUX/wavelet_pred $configfile < $testfile > $testfile.out 2>/dev/null";

open(GNU,"|gnuplot");
GNU->autoflush(1);
#print GNU "plot '$testfile.out' using 1 with linespoints, '$testfile.out' using 2 with linespoints, '$testfile.out' using 3 with linespoints\n";
print GNU "plot '$testfile.out' using 1 with linespoints, '$testfile.out' using 2 with linespoints\n";
<STDIN>;


