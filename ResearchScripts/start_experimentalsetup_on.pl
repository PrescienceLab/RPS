#!/usr/bin/env perl
#
#
#
#
#
#


$usage = "start_experimentalsetup_on.pl host tracefile start:end";


$#ARGV==2 or die "usage: $usage";

$host = $ARGV[0];
$tracepath = $ARGV[1];
$selection = $ARGV[2];

$tracetempdir = $ENV{"TRACETEMPDIR"} or die "set TRACETEMPDIR\n";
$meanpredctrlport = $ENV{"HOSTLOADPREDCTRLPORT_MEAN"} or die "set HOSTLOADPREDCTRLPORT_MEAN\n";
$lastpredctrlport = $ENV{"HOSTLOADPREDCTRLPORT_LAST"} or die "set HOSTLOADPREDCTRLPORT_LAST\n";
$meanpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_MEAN"} or die "set HOSTLOADPREDBUFFERPORT_MEAN\n";
$lastpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_LAST"} or die "set HOSTLOADPREDBUFFERPORT_LAST\n";
$sscalfile = $ENV{"SPINSERVER_CALIBRATION_FILE"} or die "set SPINSERVER_CALIBRATION_FILE\n";



@comps = split(/\//,$tracepath);
$tracefile = $comps[$#comps];
$genloadtracefile = "$tracetempdir/$tracefile";

system "select_from_binary_trace_file $selection $tracepath $genloadtracefile";

system "start_hostloadmeasure_on.pl $host";

system "start_genload_on.pl $host $genloadtracefile";

system "start_spinserver_on.pl $host --calibrationfile=$sscalfile";

system "start_hostloadpred_on.pl $host";

system "start_hostloadpred_on.pl $host --rateport=$meanpredctrlport --bufferport=$meanpredbufferport --mintest=9999999999999999 --maxtest=99999999999999999 --maxabserr=99999999999 --maxerrmissest=9999999999999999 --model=MEAN";

system "start_hostloadpred_on.pl $host --rateport=$lastpredctrlport --bufferport=$lastpredbufferport --mintest=9999999999999999 --maxtest=99999999999999999 --maxabserr=99999999999 --maxerrmissest=9999999999999999 --model=LAST";

print "Experimental setup started on host $host\n";







