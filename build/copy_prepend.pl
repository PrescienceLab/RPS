#!/usr/bin/env perl

$#ARGV >=2 or die "usage: copy_prepend.pl prependstring destinationdir files+\n";

$p=shift; chomp($p);
@files=@ARGV[0..$#ARGV-1];
$dest=$ARGV[$#ARGV]; chomp($dest);

foreach $file (@files) {
  chomp($file);
  if ($file=~/.*\/(.*)/) {
    $file=$1;
  }
  $cmd="cp $file $dest/$p$file";
  
  print "$cmd\n";
}
