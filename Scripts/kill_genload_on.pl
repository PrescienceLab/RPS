#!/usr/bin/env perl
#
#

$RSH = "ssh";
$SSCMD = "kill_genload.pl";

if ($#ARGV!=0) {
    print STDERR "kill_genload_on.pl host";
    exit;
}

$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

