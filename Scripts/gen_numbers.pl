#!/usr/bin/env perl

use RPS;

$usage=
"Generate random numbers for playing with RPS\n\n".
"usage: gen_numbers.pl sample_rate\n".RPSBanner();

$#ARGV==0 or die $usage;

$|=1;

$time=shift;
$time=1.0/$time;

for ($i=0;;$i++){
  $num=rand();
  print "$num\n";
  select(undef,undef,undef,$time);
}
