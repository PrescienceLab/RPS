#!/usr/local/bin/perl 

$numup = $ARGV[0];

open(PSXUP,"| psxup -$numup");

foreach $file (@ARGV[1 .. $#ARGV ]) {
    open(FILE,$file);
    while (<FILE>) {
	print PSXUP;
    }
    print PSXUP "showpage\n";
    close(FILE);
}


