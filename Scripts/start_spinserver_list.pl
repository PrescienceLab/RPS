#!/usr/local/bin/perl5 -w

if ($#ARGV!=0) { 
    print STDERR "start_spinserver_list.pl list\n";
    exit;
}

open(LIST,$ARGV[0]);
while (<LIST>) {
    print STDERR ;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    system "perl", "start_spinserver_on.pl", @fields;
	}
    }
}
close(LIST);
