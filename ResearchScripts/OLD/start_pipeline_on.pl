#!/usr/bin/env perl
#
#
$defaultperiod=1000000;
$defaultpipelinecmd="tcppipeline.pl";

$RSH = "ssh";


if ($#ARGV<0) { 
    print STDERR "start_lpp_on.pl host [pipelinecmd] [period_usec] [otherargs]+\n";
    exit;
}

$HOST = $ARGV[0];

if ($#ARGV>0) { 
    $PPCMD = $ARGV[1];
} else {
    $PPCMD = $defaultpipelinecmd;
}
if ($#ARGV>1) { 
    $PERIOD = $ARGV[2];
} else {
    $PERIOD = $defaultperiod;
}

if ($#ARGV>2) { 
    @otherargs=@ARGV[3 .. $#ARGV];
} else {
#    @otherargs=[];
}

$CMD = "$RSH $HOST $PPCMD $ARCH $OS $PERIOD";
foreach $other (@otherargs) { 
    $CMD.=" ".$other." ";
}

print STDERR "$CMD\n";
system $CMD;

