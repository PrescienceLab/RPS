#!/usr/local/bin/perl5 -w
#
#
#
# expects tools to be on path
#
#
#

use Getopt::Long;

$usage="start_tcppipeline.pl [--period==period] [--loadreconfigport=loadreconfigport] [--measurebufferport=measurebufferport] [--measurebufferdepth=measurebufferdepth] [--predreconfigport=predreconfigport] [--predbufferdepth=predbufferdepth] [--predbufferport==predbufferport] [--firstconnectport=firstconnectport]";


$PERIOD=1000000;
$LRPORT=9310;
$MBPORT=9311;
$PRPORT=9312;
$PBPORT=9313;
$FCPORT=10534;

$predbufferdepth=100;
$measurebufferdepth=1000;

&GetOptions(("period=i"=>\$PERIOD, "loadreconfigport=i"=>\$LRPORT,"measurebufferport=i"=>\$MBPORT,
	     "predreconfigport=i"=>\$PRPORT,"predbufferport=i"=>\$PBPORT,
	     "firstconnectport=i"=>\$FCPORT, "predbufferdepth=i"=>\$predbufferdepth, 
	     "measurebufferdepth=i"=>\$measurebufferdepth))  or die "usage: $usage\n";




$HOST=`hostname`;
chomp($HOST);

$LSC = $FCPORT;
$L2MC = $FCPORT+1;
$MBC = $FCPORT+2;
$PSC = $FCPORT+3;


print STDERR "loadserver $PERIOD server:tcp:$LRPORT connect:tcp:$LSC &; sleep(5)\n";

system "loadserver $PERIOD server:tcp:$LRPORT connect:tcp:$LSC &";
sleep(5);

print STDERR "load2measure 0 source:tcp:$HOST:$LSC connect:tcp:$L2MC &;  sleep(5)\n";
system "load2measure 0 source:tcp:$HOST:$LSC connect:tcp:$L2MC &";
sleep(5);

print STDERR "measurebuffer 1000 source:tcp:$HOST:$L2MC server:tcp:$MBPORT connect:tcp:$MBC &; sleep(5)\n";
system "measurebuffer 1000 source:tcp:$HOST:$L2MC server:tcp:$MBPORT connect:tcp:$MBC &";
sleep(5);

print STDERR "predserver source:tcp:$HOST:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT connect:tcp:$PSC &; sleep(5)\n";
system "predserver source:tcp:$HOST:$MBC source:tcp:$HOST:$MBPORT server:tcp:$PRPORT connect:tcp:$PSC &";
sleep(5);

print STDERR "predbuffer 100 source:tcp:$HOST:$PSC server:tcp:$PBPORT &;  sleep(5)\n";
system "predbuffer 100 source:tcp:$HOST:$PSC server:tcp:$PBPORT &";
sleep(5);


