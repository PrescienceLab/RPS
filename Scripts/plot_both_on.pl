#!/usr/bin/env perl

use Getopt::Long;

$usage = "plot_both_on.pl [--measurebufferport=measurebufferport] [--num=numtomeasure] [--predbufferport=predbufferport] host";

$measurebufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};
$num=1000;

&GetOptions(("measurebufferport=i"=>\$measurebufferport,"predbufferport=i"=>\$predbufferport,"num=i"=>\$num)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "get_measurements_on.pl --bufferport=$measurebufferport --num=$num $host |";

open(IN,"$CMD");
open(OUT,">_temp");
while (<IN>) {
  chomp;
  split;
  print OUT "$_[1]\t$_[2]\n";
}
close(IN);
close(OUT);


$CMD = "get_predictions_on.pl --bufferport=$predbufferport  $host |";

open(IN,"$CMD");
open(OUT,">>_temp");
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
print OUT "$time\t0.0\n";
print OUT "$timestart\t0.0\n";
print OUT "$timestart\t1.0\n";
close(IN);
close(OUT);

system "plot2.pl _temp 'Measurements on $host:$measurebufferport, \\nPredictions on $host:$predbufferport period:$period start=$timestart model=$model' 'Time' 'Value'";


system "rm -f _temp";
