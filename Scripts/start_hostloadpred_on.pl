#!/usr/bin/env perl
#
#

use RPS;

$usage="Start host load prediction on a host by sshing start_hostloadpred.pl\n\n".
"usage: start_hostloadpred_on.pl host [other args]\n".RPSBanner();

$#ARGV==0 or die $usage;

$cmd = "start_hostloadpred.pl";

$RSH = "ssh";

$HOST = $ARGV[0];
$SSCMD = $cmd;
$args = join(" ", @ARGV[1 .. $#ARGV]);

$CMD = "$RSH $HOST $SSCMD $args";

print STDERR "$CMD\n";
system $CMD;

