#!/usr/bin/perl -w

$#ARGV==3 or die "usage: make_sine.pl frequency phase samplerate numsamples\n";

$freq=shift;
$phase=shift;
$samplerate=shift;
$numsamples=shift;

for ($i=0, $t=0; $i<$numsamples;$i++,$t+=1.0/$samplerate) { 
  $val=sin($phase+$t*$freq*3.14159);
  print "$val\n";
}
