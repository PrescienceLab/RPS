#! /usr/bin/env perl
use FileHandle;
use Getopt::Long;



&GetOptions(("matlab"=>\$matlab));

$#ARGV>= 2 or die "usage: plot_step_resp.pl [--matlab] file length model\n";

$infile = shift; 
$resplen=shift;
$model = join(" ",@ARGV);

if ($matlab) {
  $CMD = "plot_impulse_resp.pl --matlab --step $infile $resplen $model";
} else {
  $CMD = "plot_impulse_resp.pl --step $infile $resplen $model";
}

system "$CMD";
