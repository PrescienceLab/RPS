#!/usr/bin/perl -w

$#ARGV==1 or die "run_view.pl type inputfile\n";

$type=shift;
$infile=shift;
$outfile="$infile.out";

system "testfilter $type $infile > $outfile";

open(IN,"$outfile");
open(OUT,">$outfile.data");

while(<IN>) {
  if (/^level=\s+\S+,\s+coef=\s+(.*)$/) {
    print OUT "$1\n";
  }
}

close(OUT);

system "(sleep 999999; echo foo) | view.pl $infile &";
push @pids, $!;
system "(sleep 999999; echo foo) | view.pl $outfile.data &";
push @pids, $!;


<STDIN>;

kill @pids;







