#!/usr/bin/perl -w

open(IN,"loadclient source:udp:239.239.239.239:9998 |");

for ($i=0;$i<100;$i++){ 
  $l=<IN>; 
  @v=split(" ",$l),  
  $x{$v[0]}++;
} 

foreach $i (sort keys %x) { print "$i: $x{$i}\n";}
