#!/usr/bin/env perl

use RPS;

$usage="Start host load prediction on a list of hosts using start_hostloadpred_on.pl\n\n".
"usage: start_hostloadpred_list.pl list\n".RPSBanner();

$#ARGV==0 or die $usage;

open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR ;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "start_hostloadpred_on.pl", @fields;
	}
    }
}
close(LIST);
