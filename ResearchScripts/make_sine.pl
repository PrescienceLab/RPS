#!/usr/bin/perl -w

$#ARGV==3 or $#ARGV==4 or die "usage: make_sine.pl frequency phase samplerate numsamplesi pause\n";

$freq=shift;
$phase=shift;
$samplerate=shift;
$numsamples=shift;
if ($#ARGV==0) {
  $pause=shift;
  $|=1;
} else {
  $pause=0;
}

for ($i=0, $t=0; $i<$numsamples;$i++,$t+=1.0/$samplerate) { 
  $val=sin($phase+$t*$freq*3.14159);
  print "$val\n";
  if ($pause>0) {
    select(undef,undef,undef,$pause);
  }
}
