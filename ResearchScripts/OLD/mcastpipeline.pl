#!/usr/bin/env perl
#
#
#
# mcastpipeline.pl period_usec loadreconfigport measurebufferport predreconfigport predbufferport first_connect_port
#
#
#

if ($#ARGV>5) {
    print STDERR "mcastpipeline.pl [period_usec] [loadreconfigport] [measurebufferport] [predreconfigport] [predbufferport] [first_connect_port]\n";
    exit;
}

$HOST=`hostname`;
$IP="239.99.99.99";
chomp($HOST);

if ($#ARGV>-1) {
    $PERIOD=$ARGV[0];
} else {
    $PERIOD=1000000;
}

if ($#ARGV>0) {
    $LRPORT=$ARGV[1];
} else {
    $LRPORT=9310;
}

if ($#ARGV>1) {
    $MBPORT=$ARGV[2];
} else {
    $MBPORT=9311;
}

if ($#ARGV>2) {
    $PRPORT=$ARGV[3];
} else {
    $PRPORT=9312;
}

if ($#ARGV>3) {
    $PBPORT=$ARGV[4];
} else {
    $PBPORT=9313;
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


print STDERR "loadserver $PERIOD server:tcp:$LRPORT target:udp:$IP:$LSC &; sleep(5)\n";

system "loadserver $PERIOD server:tcp:$LRPORT target:udp:$IP:$LSC &";
sleep(5);

print STDERR "load2measure 0 source:udp:$IP:$LSC target:udp:$IP:$L2MC &;  sleep(5)\n";
system "load2measure 0 source:udp:$IP:$LSC target:udp:$IP:$L2MC &";
sleep(5);

print STDERR "measurebuffer 1000 source:udp:$IP:$L2MC server:tcp:$MBPORT target:udp:$IP:$MBC &; sleep(5)\n";
system "measurebuffer 1000 source:udp:$IP:$L2MC server:tcp:$MBPORT target:udp:$IP:$MBC &";
sleep(5);

print STDERR "predserver source:udp:$IP:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:udp:$IP:$PSC &; sleep(5)\n";
system "predserver source:udp:$IP:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT target:udp:$IP:$PSC &";
sleep(5);

print STDERR "predbuffer 100 source:udp:$IP:$PSC server:tcp:$PBPORT &;  sleep(5)\n";
system "predbuffer 100 source:udp:$IP:$PSC server:tcp:$PBPORT &";
sleep(5);


