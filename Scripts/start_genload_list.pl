#!/usr/bin/env perl

if ($#ARGV!=0) { 
    print STDERR "start_genload_list.pl list\n";
    exit;
}

open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "perl", "start_genload_on.pl", @fields;
	}
    }
}
close(LIST);
