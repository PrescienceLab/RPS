#!/usr/bin/perl -w

$#ARGV==1 or die "usage: sum.pl file1 file2\n";

open(FILE1, $ARGV[0]);
open(FILE2, $ARGV[1]);

while (($first=<FILE1>)) {
  $second=<FILE2>;
  $sum=$first+$second;
  print "$sum\n";
}
