#!/usr/local/bin/perl5 -w
#
#

$defaultspincmd = "start_spinserver.pl";

$RSH = "ssh";

if ($#ARGV<0) { 
    print STDERR "start_spinserver_on.pl host [args]\n";
    exit;
}

$HOST = $ARGV[0];
$SSCMD = $defaultspincmd;
$args = join(" ", @ARGV[1 .. $#ARGV]);

$CMD = "$RSH $HOST $SSCMD $args";

print STDERR "$CMD\n";
system $CMD;

