#!/usr/bin/env perl
#
#
#
#
#

use RPS;
use Getopt::Long;

$period=1000000;
$rateport=$ENV{"HOSTLOADPREDCTRLPORT"};
$bufferport=$ENV{"HOSTLOADPREDBUFFERPORT"};
$numfit=300;
$numpred=180;
$bufferdepth=10;
$mintest=30;
$maxtest=300;
$maxabserr=0.01;
$maxerrmissest=5.0;
$model="AR 16";

$usage="Start host load prediction on this host\n\n".
"usage: start_hostloadpred.pl [--period=period] [--rateport=rateport]\n".
"[--bufferport=bufferport] [--numfit=numfit] [--numpred=numpred]\n".
"[--mintest=mintest] [--maxtest=maxtest] [--maxabserr=maxabserr]\n".
"[--maxerrmissest=maxerrmissest] [model=\"model\"]\n".RPSBanner();
  

&GetOptions(
( "period=i" => \$period, 
  "rateport=i" => \$rateport,
  "bufferport=i" => \$bufferport,
  "numfit=i" => \$numfit,
  "numpred=i" => \$numpred,
  "mintest=i" => \$mintest,
  "maxtest=i" => \$maxtest,
  "maxabserr=f" => \$maxabserr,
  "maxerrmissest=f" => \$maxerrmissest,
  "model=s" => \$model ,
  "bufferdepth=i" => \$bufferdepth,
  "help" => \$help
)) 

or die $usage;

if ($help) { 
  die $usage;
}

$COMMAND = "(hostloadpred server:tcp:$rateport target:stdio:stdout $period $numfit $numpred $mintest $maxtest $maxabserr $maxerrmissest $model | predbuffer $bufferdepth source:stdio:stdin server:tcp:$bufferport connect:tcp:9754) >/dev/null 2>/dev/null &";

#print STDERR "$COMMAND\n";

system "$COMMAND";


