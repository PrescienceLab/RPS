#!/usr/bin/env perl

$#ARGV==1 or die "usage: make_zeros number pause\n";

$number=shift;
$pause=shift;

$|=1;

for ($i=0;$i<$number;$i++) {
  print "0\n";
  select(undef,undef,undef,$pause);
}

