#!/usr/local/bin/perl -w
#
# start mean, last, and ar16 predictors on a set of hosts
# generate scheduling testcases and attempt to rt schedule
# them on the set of hosts

$timestamp = time;

$usage = "run_sched_experiment_aggressive.pl setupfile numsamples minint:maxint mindur:maxdur minslack:maxslack";

$#ARGV==4 or die "usage: $usage\n";

$setupfile = $ARGV[0];
@comps = split(/\//,$setupfile);
$setupname=$comps[$#comps];
$numsamples = $ARGV[1];
($minint,$maxint)  = split(/:/,$ARGV[2]);
($mindur,$maxdur)  = split(/:/,$ARGV[3]);
($minslack,$maxslack)  = split(/:/,$ARGV[4]);

$spinserverport = $ENV{"SPINSERVERPORT"} or die "set SPINSERVERPORT\n";
$ar16predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"} or die "set HOSTLOADPREDBUFFERPORT\n";
$meanpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_MEAN"} or die "set HOSTLOADPREDBUFFERPORT_MEAN\n";
$lastpredbufferport = $ENV{"HOSTLOADPREDBUFFERPORT_LAST"} or die "set HOSTLOADPREDBUFFERPORT_LAST\n";
$tracetempdir = $ENV{"TRACETEMPDIR"} or die "set TRACETEMPDIR\n";

$loadctrlport = $ENV{"HOSTLOADSERVERCTRLPORT"} or die "set HOSTLOADSERVERCTRLPORT\n";
$loadbufferport = $ENV{"HOSTLOADSERVERBUFFERPORT"} or die "set HOSTLOADSERVERBUFFERPORT\n";

open(SETUP,$setupfile);
@lines=<SETUP>;
close(SETUP);
$numhosts=0;
foreach $line (@lines) {
    if (!($line =~ /^\s*\#.*/)) {
	chomp($line);
	@fields = split(/\s+/,$line);
	if ($#fields > 0) { 
	    $hosts[$numhosts] = $fields[0];
	    @comps = split(/\//,$fields[1]);
	    $tracenames[$numhosts]=$comps[$#comps];
	    $numhosts++;
	}
    }
}

system "kill_experimentalsetup_list.pl $setupfile";

system "start_experimentalsetup_list.pl $setupfile";
sleep 300;

$outputfile = "sched_time${timestamp}_${setupname}_int${minint}:${maxint}_dur${mindur}:${maxdur}_slack${minslack}:${maxslack}.out";

$call = "test_sched $numsamples 0.95 $minint:$maxint $mindur:$maxdur $minslack:$maxslack $numhosts 3 ";
for ($i=0;$i<$numhosts;$i++) {
    $host=$hosts[$i];
    $trace=$tracenames[$i];
    $call.=" ${trace}_on_$host client:tcp:$host:$spinserverport client:tcp:$host:$loadbufferport ar16 client:tcp:$host:$ar16predbufferport mean client:tcp:$host:$meanpredbufferport last client:tcp:$host:$lastpredbufferport";
}
$call.=" > $outputfile";

print "$call\n";

system $call;

system "kill_experimentalsetup_list.pl $setupfile";


#foreach $trace (@tracenames) { 
#    print "rm $tracetempdir/$trace\n";
#    system "rm $tracetempdir/$trace";
#}


