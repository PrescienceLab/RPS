#!/usr/bin/env perl
#
#
#
# expects tools to be on path
#
# udppipeline.pl period_usec measurebufferport predreconfigport predbufferport first_connect_port
#
#
#

if ($#ARGV>5) {
    print STDERR "udppipeline.pl [period_usec] [loadreconfigport] [measurebufferport] [predreconfigport] [predbufferport] [first_connect_port]\n";
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
    $LRPORT=9311;
}

if ($#ARGV>1) {
    $MBPORT=$ARGV[2];
} else {
    $MBPORT=9312;
}

if ($#ARGV>2) {
    $PRPORT=$ARGV[3];
} else {
    $PRPORT=9313;
}

if ($#ARGV>3) {
    $PBPORT=$ARGV[4];
} else {
    $PBPORT=9314;
}

if ($#ARGV>4) { 
    $FCPORT=$ARGV[5];
} else {
    $FCPORT = 10534;
}

$LSC = $FCPORT;
$L2MC = $FCPORT+1;
$MBC = $FCPORT+2;
$PSC = $FCPORT+3;


print STDERR "loadserver $PERIOD server:tcp:$LRPORT target:udp:$HOST:$LSC &; sleep(5))\n";

system "loadserver $PERIOD server:tcp:$LRPORT target:udp:$HOST:$LSC &";
sleep(5);

print STDERR "load2measure 0 source:udp:$HOST:$LSC target:udp:$HOST:$L2MC &;  sleep(5)\n";
system "load2measure 0 source:udp:$HOST:$LSC target:udp:$HOST:$L2MC &";
sleep(5);

print STDERR "measurebuffer 1000 source:udp:$HOST:$L2MC server:tcp:$MBPORT target:udp:$HOST:$MBC &; sleep(5)\n";
system "measurebuffer 1000 source:udp:$HOST:$L2MC server:tcp:$MBPORT target:udp:$HOST:$MBC &";
sleep(5);

print STDERR "predserver source:udp:$HOST:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:udp:$HOST:$PSC &; sleep(5)\n";
system "predserver source:udp:$HOST:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:udp:$HOST:$PSC &";
sleep(5);

print STDERR "predbuffer 100 source:udp:$HOST:$PSC server:tcp:$PBPORT &;  sleep(5)\n";
system "predbuffer 100 source:udp:$HOST:$PSC server:tcp:$PBPORT &";
sleep(5);


