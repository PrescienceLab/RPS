#!/usr/bin/env perl

use Getopt::Long;

$usage = "plot_measurements_on.pl [--bufferport=bufferport] [--num=num] [--useindex] host";

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$num=1000;
$useindex=0;

&GetOptions(("bufferport=i"=>\$bufferport,"num=i"=>\$num, "useindex"=>\$useindex)) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "get_measurements_on.pl --bufferport=$bufferport --num=$num $host |";

open(IN,"$CMD");
open(OUT,">_temp");
$i=0;
while (<IN>) {
  chomp;
  split;
  if ($useindex) {
    print OUT "$i\t$_[2]\n";
  } else {
    print OUT "$_[1]\t$_[2]\n";
  }
  $i++;
}
close(IN);
close(OUT);

system "plot2.pl _temp 'Measurements on $host:$measurebufferport' 'Time' 'Value'";

system "rm -f _temp";
