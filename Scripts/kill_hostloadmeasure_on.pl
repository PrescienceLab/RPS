#!/usr/bin/env perl
#
#

use RPS;

$usage="Kill host load measurement on a host by sshing kill_hostloadmeasure.pl\n\n".
"usage: kill_hostloadmeasure_on.pl host\n".RPSBanner();

$#ARGV==0 or die $usage;

$RSH = "ssh";
$SSCMD = "kill_hostloadmeasure.pl";


$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

