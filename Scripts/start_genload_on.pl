#!/usr/local/bin/perl5 -w
#
#

$cmd = "start_genload.pl";
$RSH = "ssh";

if ($#ARGV<1) { 
    print STDERR "start_genload_on.pl host tracefile [options]\n";
    exit;
}


$HOST = $ARGV[0];
$tracefile = $ARGV[1];

$options = join(" ",@ARGV[2 .. $#ARGV]);

$CMD = "$RSH $HOST $cmd $options $tracefile";

print STDERR "$CMD\n";
system $CMD;

