#! /usr/bin/env perl
use FileHandle;


$#ARGV==3 or die 
"usage: plot_mi.pl file minlag step maxlag\n\n".
"Plots the mutual information profile of the sequence in file from\n".
"a lag of minlag to a lag of maxlag with specified step size.\n\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift;
$minlag=shift;
$step=shift;
$maxlag=shift; 

$CMD="mi $infile $minlag:$step:$maxlag > mi.txt";

system "$CMD";


open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "mutual information profile of $infile $minlag:$step:$maxlag
set xlabel "lag"
set ylabel "bits"
plot "mi.txt" using 1:2 with linespoints
DONE2
;
  print GNUPLOT $cmds;

  print "<enter> to exit, p file<enter> to print\n";

  $finish = <STDIN>;
  if ($finish =~ /^p\s+(.*)/) {
    $printfile=$1;
    print "Saving to $printfile\n";
print GNUPLOT <<DONE3
set terminal postscript eps color
set output "$printfile"
$cmds
DONE3
;
  }
  close(GNUPLOT);


exit;
