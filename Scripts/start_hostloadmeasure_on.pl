#!/usr/bin/env perl
#
#

$cmd = "start_hostloadmeasure.pl";

$RSH = "ssh";

if ($#ARGV<0) { 
    print STDERR "start_hostloadmeasure_on.pl host [args]\n";
    exit;
}

$HOST = $ARGV[0];
$SSCMD = $cmd;
$args = join(" ", @ARGV[1 .. $#ARGV]);

$CMD = "$RSH $HOST $SSCMD $args";

print STDERR "$CMD\n";
system $CMD;

