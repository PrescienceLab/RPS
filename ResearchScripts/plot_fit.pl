#! /usr/bin/env perl
use FileHandle;
use Getopt::Long;

$#ARGV>= 2 or die 
"usage: plot_fit.pl file stepsahead model\n\n".
"Plots the quality of a model fit to the data in file for\n".
"projecting the given number of steps ahead.  For information\n".
"on models, run ts_example.  \n\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift; 
$ahead=shift; $col=$ahead+1;
$model = join(" ",@ARGV);

$CMD = "test_fit $infile $ahead noconf $model 2>/dev/null";

$n=0;
open(D,"$CMD |");
while (<D>) {
  chomp;
  @vals=split;
  push @preds, $vals[$ahead];
  if ($n>=$ahead) {
    push @acts, $vals[0];
  }
  $n++;
}
close(D);

open(D,">fit.txt");
for ($i=0;$i<=$#acts;$i++) {
  print D "$acts[$i]\t$preds[$i]\n";
}
close(D);

open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "fit of $infile $model $ahead steps ahead"
set xlabel "time steps"
set ylabel "value"
plot "fit.txt" using 1 title "actual" with linespoints, "fit.txt" using 2 title "fit($ahead ahead)" with linespoints

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
