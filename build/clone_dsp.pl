#!/usr/bin/env perl

$#ARGV==1 or die "usage: clone_dsp.pl original_name new_name\n";

$orig=shift;
$new=shift;

$origstem=$orig;
$origstem=~s/\.dsp//g;
$newstem=$new;
$newstem=~s/\.dsp//g;

open(IN,$orig);
open(OUT,">$new");

while ($line=<IN>) { 
  $line=~s/$origstem/$newstem/g;
  print OUT $line;
}

close(OUT);
close(IN);

