#!/usr/bin/env perl

use Getopt::Long;
use dataaccess;
use sched_dataaccess;
use simple_stats;

@approaches = ("random", 
	       "measure", 
	       "mean", 
	       "last",
	       "ar16" );

@measures = ("fracdeadlinesmet", 
	     "fracdeadlinesmetwhenpossible", 
	     "fracinrange",
	     "numpossible");

$do_cis=0;

&GetOptions("cis"=>\$do_cis)
and 
$#ARGV == 4
or 
die "usage: sched_gen_stat.pl [--cis] measure database table tnommin:tnommax slackmin:slackmax";

$measure = $ARGV[0];
$database = $ARGV[1];
$table = $ARGV[2];
($tnommin,$tnommax) = split(/:/,$ARGV[3]);
($slackmin,$slackmax) = split(/:/,$ARGV[4]);

@col= GetColumnWhere($database,$table,"numpossible>=0","max(numpossible)");
$maxnumpossible =$col[0];

foreach $measure (@measures) { 
    foreach $approach (@approaches) { 
	$where = "predname='$approach' and tnom>=$tnommin and tnom<=$tnommax and slack>=$slackmin and slack<=$slackmax ";
	$countall = GetCountWhere($database,$table,$where);
	$numsamples{$measure}{$approach}=$countall;
	if ($measure eq "fracdeadlinesmet") {
	    $count = GetCountWhere($database, $table, $where." and deadlinemet='met'");
	    $fracmet = $countall==0 ? 0 : $count/$countall;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = $countall==0 ? 0 : 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	}
	if ($measure eq "fracdeadlinesmetwhenpossible") { 
	    $countpossible = GetCountWhere($database, $table, $where." and numpossible>0");
	    $count = GetCountWhere($database, $table, $where." and deadlinemet='met' and numpossible>0");
	    $fracmet = $countpossible==0 ? 0 : $count/$countpossible;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = $countall==0 ? 0 : 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	    
	}
	if ($measure eq "fracinrange") { 
	    $count = GetCountWhere($database, $table, $where." and inrange='in'");
	    $fracmet = $countall==0 ? 0 : $count/$countall;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = $countall==0 ? 0 : 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	}
	if ($measure eq "numpossible") { 
	    if ($approach eq "random") { 
		$avgnumpossible = $maxnumpossible;
		$stdnumpossible = 0;
	    } elsif ($approach eq "measure") { 
		$avgnumpossible = 1;
		$stdnumpossible = 0;
	    } else {
		@col = GetColumnWhere($database,$table,$where,"avg(max(1,numpossible))");

		$avgnumpossible=$col[0];
		@col = GetColumnWhere($database,$table,$where,"std(max(1,numpossible))");

		$stdnumpossible=$col[0];
	    }
	    $stat{$measure}{$approach} = $avgnumpossible;
	    $ci{$measure}{$approach} = $countall==0 ? 0 : 1.96*$stdnumpossible/sqrt($countall);
	}
    }
}


print "%measure\tdatabase\ttable\ttnommin\ttnommax\tslackmin\tslackmax";
foreach $approach (@approaches) {
    if ($do_cis) { 
	print "\t${approach}_ub\t${approach}_lb\t${approach}_avg";
    } else {
	print "\t${approach}_avg";
    }
}
print "\n";

print "$measure\t$database\t$table\t$tnommin\t$tnommax\t$slackmin\t$slackmax";
foreach $approach (@approaches) { 
    $avg=$stat{$measure}{$approach};
    $ci=$ci{$measure}{$approach};
    if ($do_cis) {
	$ub = $avg+$ci;
	$lb = $avg-$ci;
	print "\t$ub\t$lb\t$avg";
    } else {
	print "\t$avg";
    }
}
print "\n";

	    
	    
