#!/usr/bin/perl -w

$#ARGV==1 or die "comp.pl file1 file2\n";

$file1 = shift;
$file2 = shift;

open(F1,$file1);
open(F2,$file2);

while (($left=<F1>)) {
  $right=<F2>;
  chomp($left); chomp($right);
  $diff=$left-$right;
  print "$diff\n";
}

