#! /usr/bin/env perl
use FileHandle;

$#ARGV>= 3 or die 
"usage: plot2.pl file title xlabel ylabel\n\n".
"Plots a 2 column x tab y file\n".
"\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift; 
$title = shift;
$xlabel=shift;
$ylabel=shift;


open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "$title"
set xlabel "$xlabel"
set ylabel "$ylabel"
plot "$infile" using 1:2 with linespoints

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
