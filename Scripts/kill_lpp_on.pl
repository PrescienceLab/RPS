#!/usr/local/bin/perl5 -w
#
#

$RSH = "ssh";
$DIR = "/afs/cs/project/cmcl-pdinda-2/LoadMon";
$PPCMD = "kill_pipeline.pl";

if ($#ARGV!=0) {
    print STDERR "kill_lpp_on.pl host";
    exit;
}

$HOST = $ARGV[0];

$CMD = "$RSH $HOST perl $DIR/$PPCMD";

print STDERR "$CMD\n";
system $CMD;

