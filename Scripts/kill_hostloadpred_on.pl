#!/usr/bin/env perl
#
#
use RPS;

$usage="Kill host load prediction on a host by sshing kill_hostloadpred.pl\n\n".
"usage: kill_hostloadpred_on.pl host\n".RPSBanner();

$#ARGV==0 or die $usage;

$RSH = "ssh";
$SSCMD = "kill_hostloadpred.pl";


$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

