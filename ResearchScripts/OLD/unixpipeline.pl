#!/usr/bin/env perl
#
#
#
# unixpipeline.pl period_usec loadreconfigport measurebufferport predreconfigport predbufferport  
#
#
#

$defaultlrport=9310;
$defaultmbport=9311;
$defaultprport=9312;
$defaultpbport=9313;

if ($#ARGV>4) {
    print STDERR "unixpipeline.pl [period_usec] [loadreconfigport] [measurebufferport] [predreconfigport] [predbufferport]\n";
    exit;
}

$HOST=`hostname`;
chomp($HOST);


if ($#ARGV>-1) {
    $PERIOD=$ARGV[0];
} else {
    $PERIOD=1000000;
}

if ($#ARGV>0) {
    $LRPORT=$ARGV[1];
} else {
    $LRPORT=$defaultlrport;
}

if ($#ARGV>1) {
    $MBPORT=$ARGV[2];
} else {
    $MBPORT=$defaultmbport;
}

if ($#ARGV>2) {
    $PRPORT=$ARGV[3];
} else {
    $PRPORT=$defaultprport;
}

if ($#ARGV>3) {
    $PBPORT=$ARGV[4];
} else {
    $PBPORT=$defaultpbport;
}

$LS = "loadserver $PERIOD server:tcp:$LRPORT target:stdio:stdout";
$L2M= "load2measure 0 source:stdio:stdin target:stdio:stdout";
$MB = "measurebuffer 1000 source:stdio:stdin server:tcp:$MBPORT target:stdio:stdout";
$PS = "(sleep 5; predserver source:stdio:stdin source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:stdio:stdout)";
$PB = "predbuffer 100 source:stdio:stdin server:tcp:$PBPORT";

$COMMAND = "($LS | $L2M | $MB | $PS | $PB)";

print STDERR "$COMMAND\n";

system "$COMMAND&";


