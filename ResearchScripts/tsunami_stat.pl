#!/usr/bin/env perl

$#ARGV==0 or die "usage: parse_all.pl tracefilelist\n";

$tracefilelist=shift;

open(INPUT, $tracefilelist)
    or die "Couldn't open tracefilelist for reading!\n";

@levels = ("00","01","02","03","04","05","06","07","08","09","10","11","12","13");

@predictors = ("MEAN", 
               "LAST", 
               "BM(8)", 
               "MA(8)", 
               "AR(8)", 
               "AR(32)", 
               "ARMA(4,4)", 
               "ARIMA(4,1,4)", 
               "ARIMA(4,2,4)", 
               "ARFIMA(4,-1,4)");

open(OUTPUT, "> ./total.sum");

while (defined ($line = <INPUT>)) {
    chomp $line;

    $suboutfile=sprintf("%s.sum",$line);
    open(SUBOUTPUT, "> ./$suboutfile");

    foreach $level (@levels) {
	
        foreach $predictor (@predictors) {
            $infile=sprintf("%s%s-%s.res", $line, $level, $predictor);

            open(SUBINPUT, "./$infile");
            while (defined ($subline = <SUBINPUT>)) {
                chomp $subline;
                printf SUBOUTPUT "$subline\n";
                printf OUTPUT "$subline\n";
           }
           close(SUBINPUT);
        }
    }

    close(SUBOUTPUT);
}

close(INPUT);
close(OUTPUT);
