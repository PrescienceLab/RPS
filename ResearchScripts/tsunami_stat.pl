#!/usr/bin/env perl

$#ARGV==1 or die "usage: tsunami_stat.pl num_timeseries filelist\n";

$numtests=shift;
$filelist=shift;

open(INPUT, $filelist)
    or die "Couldn't open filelist for reading!\n";

while (defined ($line = <INPUT>)) {
    chomp $line;

    $suboutfile=sprintf("%s.stat",$line);
    open(SUBOUTPUT, "> ./$suboutfile");

    open(SUBINPUT, "./$line");
    $testcnt=0;
    while (defined ($measline = <SUBINPUT>)) {

    foreach $level (@levels) {
	
        foreach $predictor (@predictors) {
            $infile=sprintf("%s%s-%s.res", $line, $level, $predictor);

            open(SUBINPUT, "./$infile");
            while (defined ($subline = <SUBINPUT>)) {
                chomp $subline;
                printf SUBOUTPUT "$subline\n";
           }
           close(SUBINPUT);
        }
    }

    close(SUBOUTPUT);
}

close(INPUT);
