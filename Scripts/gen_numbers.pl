#!/usr/bin/env perl

$#ARGV==0 or die "usage: gen_numbers.pl interval\n";

$|=1;

$time=shift;

for ($i=0;;$i++){
  $num=rand();
  print "$num\n";
  select(undef,undef,undef,$time);
}
