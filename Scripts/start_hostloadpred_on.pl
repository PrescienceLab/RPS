#!/usr/local/bin/perl5 -w
#
#

$cmd = "start_hostloadpred.pl";

$RSH = "ssh";

if ($#ARGV<0) { 
    print STDERR "start_hostloadpred_on.pl host [args]\n";
    exit;
}

$HOST = $ARGV[0];
$SSCMD = $cmd;
$args = join(" ", @ARGV[1 .. $#ARGV]);

$CMD = "$RSH $HOST $SSCMD $args";

print STDERR "$CMD\n";
system $CMD;

