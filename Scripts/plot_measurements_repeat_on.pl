#!/usr/bin/env perl

use FileHandle;
use Getopt::Long;

$usage = "plot_measurements_repeat_on.pl [--bufferport=bufferport] [--num=num] [--refresh=seconds] host";

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$num=1000;

$refresh=1.0;

&GetOptions(("bufferport=i"=>\$bufferport,"num=i"=>\$num, "refresh=f"=>\$refresh)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);

while (1) {
  
  $CMD = "get_measurements_on.pl --bufferport=$bufferport --num=$num $host |";
  
  open(IN,"$CMD");
  open(OUT,">_temp");
  while (<IN>) {
    chomp;
    split;
    print OUT "$_[1]\t$_[2]\n";
  }
  close(IN);
  close(OUT);
  
  
  $infile = "_temp";
  $title = "measurements";
  $xlabel="time";
  $ylabel="measurement";
  

  $cmds = <<DONE2
set title "$title"
set xlabel "$xlabel"
set ylabel "$ylabel"
plot "$infile" using 1:2 with linespoints

DONE2
    ;
  print GNUPLOT $cmds;

  select(undef,undef,undef,$refresh);

}

  close(GNUPLOT);

system "rm -f _temp";
