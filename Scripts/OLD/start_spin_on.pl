#!/usr/local/bin/perl5 -w
#
#

$defaultarch="ALPHA";
$defaultos="DUX";
$defaultspincmd = "spinserver.pl";
$defaultport=10351;
$defaultcal="+/tmp/calibration.cal";
$RSH = "ssh";
$DIR = "/afs/cs/project/cmcl-pdinda-2/Spin";


if ($#ARGV<0) { 
    print STDERR "start_spin_on.pl host [ARCH] [OS] [calibrationfile] [port] [otherargs]+";
    exit;
}

$HOST = $ARGV[0];

if ($#ARGV>0) { 
    $ARCH = $ARGV[1];
} else {
    $ARCH = $defaultarch;
}
if ($#ARGV>1) { 
    $OS = $ARGV[2];
} else {
    $OS = $defaultos;
}
if ($#ARGV>2) { 
    $CAL = $ARGV[3];
} else {
    $PERIOD = $defaultcal;
}

if ($#ARGV>3) { 
    $PORT = $ARGV[4];
} else {
    $PORT = $defaultport;
}

if ($#ARGV>4) { 
    @otherargs=@ARGV[5 .. $#ARGV];
} else {
    @otherargs=[];
}

$CMD = "$RSH $HOST \'sh -c \"perl $DIR/$SSCMD $ARCH $OS $CAL $PORT";
foreach $other (@otherargs) { 
    $CMD.=" ".$other." ";
}

$CMD.=" > /tmp/ss.junk 2>&1\"\'";

print STDERR "$CMD\n";
system $CMD;

