#!/usr/bin/env perl

$#ARGV==1 or die "usage: wt_wrapper.pl rate_ms counter\n";

$rate=shift;
$counter=shift;

open(IN,"WatchTowerRPS -x ${rate}ms \"$counter\" |");
while (<IN>) {
  chomp; split; print $_[1], "\n";
}

