#!/usr/bin/perl -w

$#ARGV>=4 or die "usage: play_none.pl waveletnum numlevels delay numnone file [predictor]\n";

$waveletnum=shift; 
$numlevels=shift;
$delay=shift;
$numnone=shift;
$file=shift;
$pred=join(" ",@ARGV);

`generate_config_none.pl $waveletnum $numlevels $delay $numnone $pred > temp.cfg`;

system "cat temp.cfg";

`test.pl temp.cfg $file`;
