#!/usr/bin/env perl
use RPS;

$usage="Kill host load prediction on a list of hosts using kill_hostloadpred_on.pl\n\n".
"usage: kill_hostloadpred_list.pl list\n".RPSBanner();

$#ARGV==0 or die $usage;


open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "kill_hostloadpred_on.pl", $fields[0];
	}
    }
}
close(LIST);
