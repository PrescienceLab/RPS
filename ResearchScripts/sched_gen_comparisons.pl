#!/usr/local/bin/perl

use Getopt::Long;
use dataaccess;
use sched_dataaccess;

@approaches = ("random", 
	       "measure", 
	       "mean", 
	       "last",
	       "ar16" );

@measures = ("fracdeadlinesmet", 
	     "fracdeadlinesmetwhenpossible", 
	     "fracinrange",
	     "numpossible");


$do_avgs=0;
$do_numsamples=0;
$do_cis=0;
$do_absdiffs=0;
$do_reldiffs=0;
$do_avgcomps=0;
$do_cicomps=0;

&GetOptions("avgs"=>\$do_avgs,
	    "numsamples"=>\$do_numsamples,
	    "cis"=>\$do_cis,
	    "absdiffs"=>\$do_absdiffs,
	    "reldiffs"=>\$do_reldiffs,
	    "avgcomps"=>\$do_avgcomps,
	    "cicomps"=>\$do_cicomps) 
and 
$#ARGV == 3 
or 
die "usage: sched_gen_comparisons.pl [--avgs] [--numsamples] [--cis] [--absdiffs] [--reldiffs] [--avgcomps] [--cicomps] database table tnommin:tnommax slackmin:slackmax";

$database = $ARGV[0];
$table = $ARGV[1];
($tnommin,$tnommax) = split(/:/,$ARGV[2]);
($slackmin,$slackmax) = split(/:/,$ARGV[3]);

if ($do_avgs==0 and $do_numsamples==0 and $do_cis==0 and $do_absdiffs==0 and $do_reldiffs==0 and $do_avgcomps==0 and $do_cicomps==0) { 
    $do_avgs=1;
    $do_numsamples=1;
    $do_cis=1;
    $do_absdiffs=1;
    $do_reldiffs=1;
    $do_avgcomps=1;
    $do_cicomps=1;
}



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
	    $ci{$measure}{$approach} = 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	}
	if ($measure eq "fracdeadlinesmetwhenpossible") { 
	    $countpossible = GetCountWhere($database, $table, $where." and numpossible>0");
	    $count = GetCountWhere($database, $table, $where." and deadlinemet='met' and numpossible>0");
	    $fracmet = $countpossible==0 ? 0 : $count/$countpossible;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	    
	}
	if ($measure eq "fracinrange") { 
	    $count = GetCountWhere($database, $table, $where." and inrange='in'");
	    $fracmet = $countall==0 ? 0 : $count/$countall;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
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
	    $ci{$measure}{$approach} = 1.96*$stdnumpossible/sqrt($countall);
	}
    }
}

if ($do_avgs) { 
# print the raw avgs table
    print "Raw averages table\n";
    print "%measure";
    foreach $approach (@approaches) { 
	print "\t$approach";
    }
    print "\n";
    foreach $measure (@measures) { 
	print $measure;
	foreach $approach (@approaches) { 
	    printf "\t%g", $stat{$measure}{$approach};
	}
	print "\n";
    }
    print "\n\n";
}

if ($do_numsamples) { 
# print the numsamples table
    print "Raw numsamples table\n";
    print "%measure";
    foreach $approach (@approaches) { 
	print "\t$approach";
    }
    print "\n";
    foreach $measure (@measures) { 
	print "$measure\t\t";
	foreach $approach (@approaches) { 
	    printf "\t%g" , $numsamples{$measure}{$approach};
	}
	print "\n";
    }
    print "\n\n";
}


if ($do_cis) { 
# print the ci tables;
    print "%measure";
    foreach $approach (@approaches) { 
	print "\t$approach";
    }
    print "\n";
    foreach $measure (@measures) { 
	print "$measure";
	foreach $approach (@approaches) { 
	    $min = $stat{$measure}{$approach} - $ci{$measure}{$approach};
	    $max = $stat{$measure}{$approach} + $ci{$measure}{$approach};
	    printf "\t[%g,%g]",$min,$max;
	}
	print "\n";
    }
    print "\n\n";
}

if ($do_absdiffs) { 
# print the absolute difference tables
    foreach $measure (@measures) { 
	print "$measure absolute difference table (row-col)\n";
	foreach $approachcol (@approaches) { 
	    print "\t$approachcol";
	}
	print "\n";
	foreach $approachrow (@approaches) { 
	    $rowval = $stat{$measure}{$approachrow};
	    print "$approachrow";
	    foreach $approachcol (@approaches) { 
		$colval = $stat{$measure}{$approachcol};
		$diff = $rowval-$colval;
		printf "\t%g", $diff;
	    }
	    print "\n";
	}
	print "\n\n";
    }
    print "\n\n";
}

if ($do_reldiffs) { 
# print the relative difference tables
    foreach $measure (@measures) { 
	print "$measure relative difference table 100*(row-col)/(col)\n";
	foreach $approachcol (@approaches) { 
	    print "\t$approachcol";
	}
	print "\n";
	foreach $approachrow (@approaches) { 
	    $rowval = $stat{$measure}{$approachrow};
	    print "$approachrow";
	    foreach $approachcol (@approaches) { 
		$colval = $stat{$measure}{$approachcol};
		$reldiff = $colval== 0 ?  ($rowval>=0 ? "+Inf" : "-Inf") : 100.0*($rowval-$colval)/$colval;
		if ($reldif =~ /Inf/) { 
		    print "\t$reldiff";
		} else {
		    printf "\t%g",$reldiff;
		}
	    }
	    print "\n";
	}
	print "\n\n";
    }
    print "\n\n";
}


if ($do_avgcomps) { 
# print comparison of raw averages tables
    foreach $measure (@measures) { 
	print "$measure comparison of raw averages table\n";
	foreach $approachcol (@approaches) { 
	    print "\t$approachcol";
	}
	print "\n";
	foreach $approachrow (@approaches) { 
	    $rowval = $stat{$measure}{$approachrow};
	    print "$approachrow";
	    foreach $approachcol (@approaches) { 
		$colval = $stat{$measure}{$approachcol};
		if ($rowval < $colval) {
		    $char = "<";
		} elsif ($rowval > $colval) { 
		    $char = ">";
		} else {
		    $char = "=";
		}
		print "\t$char";
	    }
	    print "\n";
	}
	print "\n\n";
    }
}

if ($do_cicomps) { 
# print the summary tables
    foreach $measure (@measures) { 
	print "$measure t-test comparison table\n";
	foreach $approachcol (@approaches) { 
	    print "\t$approachcol";
	}
	print "\n";
	foreach $approachrow (@approaches) { 
	    $minrow = $stat{$measure}{$approachrow} - $ci{$measure}{$approachrow};
	    $maxrow = $stat{$measure}{$approachrow} + $ci{$measure}{$approachrow};
	    print "$approachrow";
	    foreach $approachcol (@approaches) { 
		$mincol = $stat{$measure}{$approachcol} - $ci{$measure}{$approachcol};
		$maxcol = $stat{$measure}{$approachcol} + $ci{$measure}{$approachcol};
		if ($maxrow < $mincol) {
		    $char = "<";
		} elsif ($minrow > $maxcol) { 
		    $char = ">";
		} else {
		    $char = "=";
		}
		print "\t$char";
	    }
	    print "\n";
	}
	print "\n\n";
    }
}
	    
	    
