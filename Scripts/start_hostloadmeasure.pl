#!/usr/local/bin/perl -w

use Getopt::Long;

$loadctrlport = $ENV{"HOSTLOADSERVERCTRLPORT"} or die "set HOSTLOADSERVERCTRLPORT\n";
$loadbufferport = $ENV{"HOSTLOADSERVERBUFFERPORT"} or die "set HOSTLOADSERVERBUFFERPORT\n";
$period = 1000000;
$bufferdepth=1000;

&GetOptions(
( "period=i" => \$period, 
  "ctrlport=i" => \$loadctrlport,
  "bufferport=i" => \$loadbufferport,
  "bufferdepth=i" => \$bufferdepth,
)) 
or die "usage: start_hostloadmeasure.pl [--period=period] [--ctrlport=ctrlport] [--bufferport=bufferport] [--bufferdepth=bufferdepth]\n";
  

system "(loadserver $period server:tcp:$loadctrlport target:stdio:stdout | loadbuffer 1000 source:stdio:stdin server:tcp:$loadbufferport) > /dev/null 2>&1 &";


