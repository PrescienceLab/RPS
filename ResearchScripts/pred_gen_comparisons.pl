#!/usr/local/bin/perl

use Getopt::Long;
use dataaccess;
use predci_dataaccess;
use simple_stats;

@approaches = ("mean", 
	       "last",
	       "ar16" );

@measures = ("fracinci", 
	     "relabserr", 
	     "ciwidth",
	     "tactvstexpr2");


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
$#ARGV == 2
or 
die "usage: pred_gen_comparisons.pl [--avgs] [--numsamples] [--cis] [--absdiffs] [--reldiffs] [--avgcomps] [--cicomps] database table tnommin:tnommax";

$database = $ARGV[0];
$table = $ARGV[1];
($tnommin,$tnommax) = split(/:/,$ARGV[2]);

if ($do_avgs==0 and $do_numsamples==0 and $do_cis==0 and $do_absdiffs==0 and $do_reldiffs==0 and $do_avgcomps==0 and $do_cicomps==0) { 
    $do_avgs=1;
    $do_numsamples=1;
    $do_cis=1;
    $do_absdiffs=1;
    $do_reldiffs=1;
    $do_avgcomps=1;
    $do_cicomps=1;
}



foreach $measure (@measures) { 
    foreach $approach (@approaches) { 
	$where = "model='$approach' and tnom>=$tnommin and tnom<=$tnommax ";
	$countall = GetCountWhere($database,$table,$where);
	$numsamples{$measure}{$approach}=$countall;
	if ($measure eq "fracinci") {
	    $count = GetCountWhere($database, $table, $where." and tact>=tlb and tact<=tub");
	    $fracmet = $countall==0 ? 0 : $count/$countall;
	    $stat{$measure}{$approach} = $fracmet;
	    $ci{$measure}{$approach} = 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
	}
	if ($measure eq "relabserr") { 
	    @col = GetColumnWhere($database,$table,$where,"avg(abs(texp-tact)/texp)");
	    $avgrelabserr=$col[0];
	    @col = GetColumnWhere($database,$table,$where,"std(abs(texp-tact)/texp)");
	    $stdrelabserr=$col[0];
	    $stat{$measure}{$approach} = $avgrelabserr;
	    $ci{$measure}{$approach} = 1.96*$stdrelabserr/sqrt($countall);
	}
	if ($measure eq "ciwidth") { 
	    @col = GetColumnWhere($database,$table,$where,"avg(tub-tlb)");
	    $avgciwidth=$col[0];
	    @col = GetColumnWhere($database,$table,$where,"std(tub-tlb)");
	    $stdciwidth=$col[0];
	    $stat{$measure}{$approach} = $avgciwidth;
	    $ci{$measure}{$approach} = 1.96*$stdciwidth/sqrt($countall);
	}
	if ($measure eq "tactvstexpr2") { 
	    @texp = GetColumnWhere($database,$table,$where,"texp");
	    @tact = GetColumnWhere($database,$table,$where,"tact");
	    ($r2,$m,$b) = LeastSquaresLine(\@texp,\@tact);
	    ($r2lb,$r2ub) = r2ci($r2,$countall);
	    $stat{$measure}{$approach} = $r2;
	    $ci{$measure}{$approach} = ($r2ub-$r2lb)/2;
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
	    
	    
