#!/usr/local/bin/perl5 -w
#
#

$RSH = "ssh";
$SSCMD = "kill_hostloadpred.pl";

if ($#ARGV!=0) {
    print STDERR "kill_hostloadpred_on.pl host\n";
    exit;
}

$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

