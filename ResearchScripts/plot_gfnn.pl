#! /usr/bin/env perl
use FileHandle;


$#ARGV==4 or die 
"usage: plot_gfnn.pl file period mindim step maxdim\n\n".
"Plots the global false nearest neighbor profile of the sequence\n".
"in file from a dimension of mindim to a dimension of maxdim\n".
"according to the specified step size.  A period of 1 means every\n".
"sample is used.  A period of 2 means every second sample, and so on\n".
"\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift;
$period =shift;
$mindim=shift;
$step=shift;
$maxdim=shift; 

$CMD="gfnn $infile $period $mindim:$step:$maxdim > gfnn.txt";

system "$CMD";


open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "global false nearest neighbors profile of $infile $mindim:$step:$maxdim period=$period"
set xlabel "dimensions"
set ylabel "average false neighbors"
plot "gfnn.txt" using 1:2 with linespoints
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
es
