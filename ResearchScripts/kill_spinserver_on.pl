#!/usr/bin/env perl
#
#

$RSH = "ssh";
$SSCMD = "kill_spinserver.pl";

if ($#ARGV!=0) {
    print STDERR "kill_spinserver_on.pl host";
    exit;
}

$HOST = $ARGV[0];

$CMD = "$RSH $HOST $SSCMD";

print STDERR "$CMD\n";
system $CMD;

