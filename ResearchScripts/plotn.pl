#! /usr/bin/env perl
use FileHandle;

$#ARGV>= 4 or die 
"usage: plotn.pl file title xlabel ylabel outfile\n\n".
"Plots a n column x tab y file : x y1 y2 y3 ...\n".
"\nPart of RPS (http://www.cs.northwestern.edu/~RPS)\n\n";

$infile = shift; 
$title = shift;
$xlabel=shift;
$ylabel=shift;
$outfile=shift;


# see how many columns there are in the file
# assume each line has same number of columns

open(TEMP,$infile);
$line=<TEMP>; chomp($line);
close(TEMP);
@junk=split(/\s+/,$line);
$numy=$#junk;


open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
$cmds = <<DONE2
set title "$title"
set xlabel "$xlabel"
set ylabel "$ylabel"
set terminal postscript eps color
set output "$outfile"
DONE2
;

$cmds .="plot ";

for ($i=0;$i<$numy;$i++) {
  if ($i>0) {
    $cmds.=", ";
  }
  $col=2+$i;
  $cmds.="\"$infile\" using 1:$col with linespoints";
}

$cmds.="\nquit\n";

print STDERR "Executing the following commands\n$cmds\n";
print GNUPLOT $cmds;
close(GNUPLOT);

exit;
