#!/usr/bin/env perl

$#ARGV==0  or die "usage: run_gui_list.pl list\n";

open(LIST,$ARGV[0]);
$numhosts=0;
while (<LIST>) {
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    $host[$numhosts] = $fields[0];
	    $otheropts[$numhosts] = join(" ", @fields[1 .. $#fields]);
	    $numhosts++;
	}
    }
}
close(LIST);



for ($i=0;$i<$numhosts;$i++) {
    $CMD = "run_gui_on.pl $otheropts[$i] $host[$i]";
    system $CMD;
}
