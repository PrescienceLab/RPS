#! /usr/bin/env perl
use FileHandle;

$#ARGV>= 2 or die 
"usage: plot_project.pl file stepsahead model\n\n".
"Fits the model to the data in the file and then predicts\n".
"for the desired number of steps ahead\n".
"For information on models, run ts_example.  \n\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift; 
$ahead=shift;
$model = join(" ",@ARGV);

$CMD = "test_project $infile $ahead $model > project.txt 2>/dev/null";

system $CMD;


open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "projection of $infile $model $ahead steps ahead"
set xlabel "time steps"
set ylabel "value or prediction"
plot "project.txt" using 1:2 title "actual" with linespoints, "project.txt" using 1:3 title "projection" with linespoints

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
