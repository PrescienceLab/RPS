#!/usr/local/bin/perl -w
#
# Generate testcases that show exectime 
# predictions for mean, last, and default (ar16) models
# running on a host with load being played from a trace
#
#


$timestamp = time;

$usage = "run_predci_experiment.pl host trace tracestart:traceend numsamples minint:maxint mindur:maxdur mintau:maxtau";

$#ARGV==6 or die "usage: $usage\n";

$host = $ARGV[0];
$trace = $ARGV[1];
@comps = split(/\//,$trace);
$tracename=$comps[$#comps];
($tracestart,$traceend)  = split(/:/,$ARGV[2]);
$numsamples = $ARGV[3];
($minint,$maxint)  = split(/:/,$ARGV[4]);
($mindur,$maxdur)  = split(/:/,$ARGV[5]);
($mintau,$maxtau)  = split(/:/,$ARGV[6]);

$spinserverport = $ENV{"SPINSERVERPORT"} or die "set SPINSERVERPORT\n";
$ar16predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"} or die "set HOSTLOADPREDBUFFERPORT\n";
$meanpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_MEAN"} or die "set HOSTLOADPREDBUFFERPORT_MEAN\n";
$lastpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_LAST"} or die "set HOSTLOADPREDBUFFERPORT_LAST\n";
$tracetempdir = $ENV{"TRACETEMPDIR"} or die "set TRACETEMPDIR\n";

$tracetempfile = "$tracetempdir/$tracename";

system "kill_experimentalsetup_on.pl $host";


system "start_experimentalsetup_on.pl $host $trace $tracestart:$traceend";
sleep 300;

$outputfile = "predci_time${timestamp}_${host}_${tracename}_trace${tracestart}:${traceend}_int${minint}:${maxint}_dur${mindur}:${maxdur}_tau${mintau}:${maxtau}.out";

$call = "test_pred_vartau $numsamples $minint:$maxint $mindur:$maxdur $mintau:$maxtau ar16 client:tcp:$host:$ar16predbufferport client:tcp:$host:$spinserverport mean client:tcp:$host:$meanpredbufferport client:tcp:$host:$spinserverport last client:tcp:$host:$lastpredbufferport client:tcp:$host:$spinserverport > $outputfile";

print "$call\n";

system $call;

system "kill_experimentalsetup_on.pl $host";

system "rm $tracetempfile";
