#!/usr/bin/env perl
#
#
#
# unixpipeline_withconnects.pl ARCH OS period_usec measurebufferport predreconfigport predbufferport predconnectport
#
#
#

$DIR =`printenv PPSDIR`;
chomp($DIR);
$DIR.="/LoadMon";

$defaultmbport=9311;
$defaultprport=9312;
$defaultpbport=9313;

$defaultpcport=9355;

if ($#ARGV<1 || $#ARGV>5) {
    print STDERR "unixpipeline_withconnects.pl ARCH OS [period_usec] [measurebufferport] [predreconfigport] [predbufferport] [predconnectport]\n";
    exit;
}

$ARCH=$ARGV[0];
$OS=$ARGV[1];
$HOST=`hostname`;
chomp($HOST);


if ($#ARGV>1) {
    $PERIOD=$ARGV[2];
} else {
    $PERIOD=1000000;
}

if ($#ARGV>2) {
    $MBPORT=$ARGV[3];
} else {
    $MBPORT=$defaultmbport;
}

if ($#ARGV>3) {
    $PRPORT=$ARGV[4];
} else {
    $PRPORT=$defaultprport;
}

if ($#ARGV>4) {
    $PBPORT=$ARGV[5];
} else {
    $PBPORT=$defaultpbport;
}

if ($#ARGV>5) {
    $PCPORT=$ARGV[6];
} else {
    $PCPORT=$defaultpcport;
}

$DIR.="/bin/$ARCH/$OS";

$LS = "loadserver $PERIOD target:file:stdout";
$L2M= "load2measure 0 source:file:stdin target:file:stdout";
$MB = "measurebuffer 1000 source:file:stdin server:tcp:$MBPORT target:file:stdout";
$PS = "(sleep 5; predserver source:file:stdin source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:file:stdout )";
$PB = "predbuffer 100 source:file:stdin server:tcp:$PBPORT connect:tcp:$PCPORT ";

$COMMAND = "(PATH=\$PATH\":$DIR\"; export PATH; $LS | $L2M | $MB | $PS | $PB)";

print STDERR "$COMMAND\n";

system "$COMMAND&";


