#!/usr/bin/env perl

use Getopt::Long;

$usage = "plot_predictions_on.pl [--bufferport=bufferport] host";

$bufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};

&GetOptions(("bufferport=i"=>\$bufferport,"num=i"=>\$num)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "get_predictions_on.pl --bufferport=$bufferport  $host |";

open(IN,"$CMD");
open(OUT,">_temp");
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

system "plot2.pl _temp 'Predictions on $host:$bufferport period:$period start=$timestart model=$model' 'Time' 'Value'";

system "rm -f _temp";
