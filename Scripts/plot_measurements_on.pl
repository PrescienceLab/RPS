#!/usr/bin/env perl

use Getopt::Long;

$usage = "plot_measurements_on.pl [--bufferport=bufferport] [--num=num] host";

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$num=1000;

&GetOptions(("bufferport=i"=>\$bufferport,"num=i"=>\$num)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

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

system "plot2.pl _temp 'Measurements on $host:$measurebufferport' 'Time' 'Value'";

system "rm -f _temp";
