#!/usr/bin/env perl
#
#

$RSH = "ssh";
$SSCMD = "kill_hostloadmeasure.pl";

if ($#ARGV!=0) {
    print STDERR "kill_hostloadmeasure_on.pl host\n";
    exit;
}

$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

