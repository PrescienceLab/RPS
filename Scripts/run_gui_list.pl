#!/usr/bin/env perl

use RPS;

$usage="Run the Java GUI on a list of hosts and sources using run_gui_on.pl\n\n".
"usage: run_gui_list.pl list\n".RPSBanner();

$#ARGV==0  or die $usage;

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
