#!/usr/bin/env perl
#
#
#
#
#

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
)) 

or die "usage: start_hostloadpred.pl [--period=period] [--rateport=rateport] [--bufferport=bufferport] [--numfit=numfit] [--numpred=numpred] [--mintest=mintest] [--maxtest=maxtest] [--maxabserr=maxabserr] [--maxerrmissest=maxerrmissest] [model=\"model\"]\n";
  

$COMMAND = "(hostloadpred server:tcp:$rateport target:stdio:stdout $period $numfit $numpred $mintest $maxtest $maxabserr $maxerrmissest $model | predbuffer $bufferdepth source:stdio:stdin server:tcp:$bufferport) &";

#print STDERR "$COMMAND\n";

system "$COMMAND";


