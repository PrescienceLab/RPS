#!/usr/bin/env perl

use Getopt::Long;

$numtoprint=-1;

$#ARGV==0 or die "usage: get_measurements_list.pl list\n";

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
    $CMD = "get_measurements_on.pl $otheropts[$i] $host[$i]";
    open(MEASURE,"$CMD |");
    $data=<MEASURE>;
    close(PRED);
    @measure = split(/\s+/,$data);
    print "$host[$i]: $measure[2]\n";
}
