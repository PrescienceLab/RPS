#!/usr/bin/env perl

use POSIX;
use FileHandle;
use Getopt::Long;

$usage = "plot_predictions_repeat_on.pl [--bufferport=bufferport] [--refresh=seconds] host";

$bufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};

$refresh=1.0;

$tempfile="_temp".getpid();

&GetOptions(("bufferport=i"=>\$bufferport,"num=i"=>\$num,"refresh=f"=>\$refresh)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);

while (1) {
  $CMD = "get_predictions_on.pl --bufferport=$bufferport  $host |";
  
  open(IN,"$CMD");
  open(OUT,">$tempfile");
  $line=<IN>;
  $line=~/.*F\((.*)Hz\).*M\((.*)secs\).*human-name=\'(.*)\'/;
  $period=1.0/$1;
  $timestart=$2;
  $model=$3;
  while (<IN>) {
    chomp;
    split;
    $time=$_[0]*$period+$timestart;
    $value=$_[1];
    print OUT "$time\t$value\n";
  }
  close(IN);
  close(OUT);

  $infile = "$tempfile";
  $title = "Predictions on $host:$bufferport period:$period start=$timestart model=$model";
  $xlabel="Time";
  $ylabel="Value";
  

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

system "rm -f $tempfile";
