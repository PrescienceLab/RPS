#!/usr/bin/env perl
#
#

use RPS;

$usage="Start host load measurement on a host by sshing start_hostloadmeasure.pl\n\n".
"usage: start_hostloadmeasure_on.pl host [other args]\n".RPSBanner();

$#ARGV==0 or die $usage;

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

