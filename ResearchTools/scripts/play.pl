#!/usr/bin/perl -w

$#ARGV>=3 or die "usage: play.pl waveletnum numlevels delay file [predictor]\n";

$waveletnum=shift; 
$numlevels=shift;
$delay=shift;
$file=shift;
$pred=join(" ",@ARGV);

`generate_config.pl $waveletnum $numlevels $delay $pred > temp.cfg`;

system "cat temp.cfg";

`test.pl temp.cfg $file`;
