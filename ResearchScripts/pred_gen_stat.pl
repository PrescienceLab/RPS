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


$do_cis=0;

&GetOptions("cis"=>\$do_cis)
and 
$#ARGV == 3
or 
die "usage: pred_gen_stat.pl [--cis] measure database table tnommin:tnommax";

$measure = $ARGV[0];
$database = $ARGV[1];
$table = $ARGV[2];
($tnommin,$tnommax) = split(/:/,$ARGV[3]);


foreach $approach (@approaches) { 
    $where = "model='$approach' and tnom>=$tnommin and tnom<=$tnommax ";
    $countall = GetCountWhere($database,$table,$where);
    $numsamples{$measure}{$approach}=$countall;
    if ($measure eq "fracinci") {
	$count = GetCountWhere($database, $table, $where." and tact>=tlb and tact<=tub");
	$fracmet = $countall==0 ? 0 : $count/$countall;
	$stat{$measure}{$approach} = $fracmet;
	$ci{$measure}{$approach} = 1.96*sqrt($fracmet*(1-$fracmet)/$countall);
    } elsif ($measure eq "relabserr") { 
	@col = GetColumnWhere($database,$table,$where,"avg(abs(texp-tact)/texp)");
	$avgrelabserr=$col[0];
	@col = GetColumnWhere($database,$table,$where,"std(abs(texp-tact)/texp)");
	$stdrelabserr=$col[0];
	$stat{$measure}{$approach} = $avgrelabserr;
	$ci{$measure}{$approach} = 1.96*$stdrelabserr/sqrt($countall);
    } elsif ($measure eq "ciwidth") { 
	@col = GetColumnWhere($database,$table,$where,"avg(tub-tlb)");
	$avgciwidth=$col[0];
	@col = GetColumnWhere($database,$table,$where,"std(tub-tlb)");
	$stdciwidth=$col[0];
	$stat{$measure}{$approach} = $avgciwidth;
	$ci{$measure}{$approach} = 1.96*$stdciwidth/sqrt($countall);
    } elsif ($measure eq "tactvstexpr2") { 
	@texp = GetColumnWhere($database,$table,$where,"texp");
	@tact = GetColumnWhere($database,$table,$where,"tact");
	($r2,$m,$b) = LeastSquaresLine(\@texp,\@tact);
	($r2lb,$r2ub) = r2ci($r2,$countall);
	$stat{$measure}{$approach} = $r2;
	$ci{$measure}{$approach} = ($r2ub-$r2lb)/2;
    } else {
	die "UNKNOWN MEASURE!\n";
    }
}


print "%measure\tdatabase\ttable\ttnommin\ttnommax";
foreach $approach (@approaches) {
    if ($do_cis) { 
	print "\t${approach}_ub\t${approach}_lb\t${approach}_avg";
    } else {
	print "\t${approach}_avg";
    }
}
print "\n";

print "$measure\t$database\t$table\t$tnommin\t$tnommax";
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

	    
	    
