#!/usr/bin/env perl

use POSIX;
use FileHandle;
use Getopt::Long;

$usage = "w_plot.pl inputbufferport  outputbufferport num refresh host\n";
$#ARGV==4 or die $usage;

$inputport=shift;
$outputport=shift;
$num=shift;
$refresh=shift;
$host=shift;

$infile="_tempin".getpid();
$outfile="_tempout".getpid();

open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);

while (1) {
  
  $CMD = "get_measurements_on.pl --bufferport=$inputport --num=$num $host |";
  
  open(IN,"$CMD");
  open(OUT,">$infile");
  $i=0;
  while (<IN>) {
    chomp;
    split;
    print OUT "$i\t$_[2]\n";
    $i++;
  }
  close(IN);
  close(OUT);

  $CMD = "get_measurements_on.pl --bufferport=$outputport --num=$num $host |";
  
  open(IN,"$CMD");
  open(OUT,">$outfile");
  $i=0;
  while (<IN>) {
    chomp;
    split;
    print OUT "$i\t$_[2]\n";
    $i++;
  }
  close(IN);
  close(OUT);
  
  
  $title = "measurements";
  $xlabel="time";
  $ylabel="measurement";
  

  $cmds = <<DONE2
set title "$title"
set xlabel "$xlabel"
set ylabel "$ylabel"
plot "$infile" using 1:2 with linespoints, "$outfile" using 1:2 with linespoints

DONE2
    ;
  print GNUPLOT $cmds;

  select(undef,undef,undef,$refresh);

}

  close(GNUPLOT);

system "rm -f _temp";
