#!/usr/bin/env perl

use RPS;

use Getopt::Long;

$loadctrlport = $ENV{"HOSTLOADSERVERCTRLPORT"} or die "set HOSTLOADSERVERCTRLPORT\n";
$loadbufferport = $ENV{"HOSTLOADSERVERBUFFERPORT"} or die "set HOSTLOADSERVERBUFFERPORT\n";
$measurebufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"} or die "set HOSTLOADMEASUREBUFFERPORT\n";

$period = 1000000;
$bufferdepth=1000;

$usage= "Start host load measurement on this host\n\n".
"usage: start_hostloadmeasure.pl [--period=period] [--ctrlport=ctrlport]\n".
"[--bufferport=bufferport] [--bufferdepth=bufferdepth]\n".RPSBanner();

&GetOptions(
( "period=i" => \$period, 
  "ctrlport=i" => \$loadctrlport,
  "bufferport=i" => \$loadbufferport,
  "bufferdepth=i" => \$bufferdepth,
  "help" => \$help
)) 
or die $usage;
  
die $usage if $help;


system "(loadserver $period server:tcp:$loadctrlport target:stdio:stdout | loadbuffer $bufferdepth source:stdio:stdin server:tcp:$loadbufferport target:stdio:stdout | load2measure 0 source:stdio:stdin target:stdio:stdout | measurebuffer $bufferdepth source:stdio:stdin server:tcp:$measurebufferport ) > /dev/null 2>&1 &";


