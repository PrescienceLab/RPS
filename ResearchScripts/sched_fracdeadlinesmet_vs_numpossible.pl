#!/usr/local/bin/perl
use sched_dataaccess;
use dataaccess;


@models = ( "random", "measure", "mean", "last", "ar16" );


$#ARGV==3
    or
die "usage: sched_fracdeadlinesmet_vs_numpossible.pl database table tnomrange slackrange";

$database = $ARGV[0];
$table = $ARGV[1];
($tnommin,$tnommax) = split(/:/,$ARGV[2]);
($slackmin,$slackmax) = split(/:/,$ARGV[3]);


@col= GetColumnWhere($database,$table,"numpossible>=0","max(numpossible)");
$maxnumpossible =$col[0];

print "numpos";
foreach $approach (@models) { 
    print "\t$approach";
}
print "\n";

for ($numpossible=0;$numpossible<=$maxnumpossible;$numpossible++) { 
    print "$numpossible";
    foreach $approach (@models) { 
	$where = "predname='$approach' and tnom>=$tnommin and tnom<=$tnommax and slack>=$slackmin and slack<=$slackmax and numpossible=$numpossible";

	$countall = GetCountWhere($database,$table,$where);
	$count = GetCountWhere($database,$table,$where." and deadlinemet='met'");
	$frac = $countall==0 ? 0: $count/$countall;
	print "\t$frac";
    }
    print "\n";
}


