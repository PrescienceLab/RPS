#!/usr/bin/env perl
use RPS;

$usage="Kill host load measurement on a list of hosts using kill_hostloadmeasure_on.pl\n\n".
"usage: kill_hostloadmeasure_list.pl list\n".RPSBanner();

$#ARGV==0 or die $usage;

open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "kill_hostloadmeasure_on.pl", $fields[0];
	}
    }
}
close(LIST);
