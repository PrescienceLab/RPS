#!/usr/bin/env perl
#
#
#
# reconfigure_list.pl list fitsamples predhorizon modeltype [modelparms]+
#
#
#

if ($#ARGV<3) {
    print STDERR "reconfigure_host.pl list fitsamples predhorizon modeltype [modelparms]+\n";
    exit;
}

$list=$ARGV[0];
$fitsamples=$ARGV[1];
$predhorizon=$ARGV[2];
@modelstuff=@ARGV[3 .. $#ARGV];

open(LIST,$list);
while (<LIST>) {
    print STDERR ;
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    $host = $fields[0];
	    $port = $fields[6];
	    $CMD = "predreconfig source:tcp:$host:$port $fitsamples $predhorizon";
	    foreach $modelthing (@modelstuff) {
		$CMD.=" $modelthing";
	    }
	    print STDERR "$CMD\n";
	    system $CMD;
	}
    }
}
