#!/usr/bin/env perl

use RPS;

$usage="Start host load measurement on a list of hosts using start_hostloadmeasure_on.pl\n\n".
"usage: start_hostloadmeasure_list.pl list [other args]\n".RPSBanner();

$#ARGV==0 or die $usage;


open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR ;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "perl", "start_hostloadmeasure_on.pl", @fields;
	}
    }
}
close(LIST);
