#!/usr/bin/env perl

if ($#ARGV!=0) { 
    print STDERR "kill_experimentalsetup_list.pl list\n";
    exit;
}

open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "kill_experimentalsetup_on.pl", $fields[0];
	}
    }
}
close(LIST);
