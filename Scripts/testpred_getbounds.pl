#!/usr/local/bin/perl5 -w


sub Sum {
    my $listref= shift;
    my $sum=0;
    my $i;
    for ($i=0;$i<=$#$listref;$i++) {
	$sum+=$listref->[$i];
    }
    return $sum;
}

sub Mean {
    my $listref=shift;
    return Sum($listref)/($#$listref+1);
}

sub Variance {
    my $listref = shift;
    my $mean = Mean($listref);
    my $i;
    my $sum2=0.0;
    for ($i=0;$i<=$#$listref;$i++) { 
	$sum2 += ($listref->[$i]-$mean)**2;
    }
    return $sum2 / ($#$listref);
}

sub StdDev {
    return sqrt(Variance(@_));
}

sub Min {
    my ($a,$b) = @_;
    return $a<$b ? $a : $b;
}

sub LeastSquaresLine {
     my $xlistref = shift;
     my $ylistref = shift;
     my $n=Min($#$xlistref,$#$ylistref)+1;
     my $sumxy=0.0;
     my $sumx2=0.0;
     my $sumy2=0.0;
     my $meanx=0.0;
     my $meany=0.0;
     my $i;
     for ($i=0;$i<$n;$i++) {
	 $meanx+=$xlistref->[$i];
         $meany+=$ylistref->[$i];
         $sumxy+=($xlistref->[$i])*($ylistref->[$i]);
         $sumx2+=($xlistref->[$i])**2;
	 $sumy2+=($ylistref->[$i])**2;
     }
     $meanx/=$n;
     $meany/=$n;
     my $m = ($sumxy - $n*$meanx*$meany) / ($sumx2 - $n*$meanx**2);
     my $b = $meany - $m * $meanx;
     my $SST=0;
     my $SSE=0;
     for ($i=0;$i<$n;$i++) {
	 $SST+=($ylistref->[$i]-$meany)**2;
	 $SSE+=($ylistref->[$i]-($m*$xlistref->[$i]+$b))**2;
     }
     my $r2 = ($SST-$SSE)/$SST;
     return ($r2,$m,$b);
 }

sub r2ci {
    my ($r2, $n) = @_;
    my $z = 0.5*log((1+$r2)/(1-$r2));
    my $se = 1/sqrt($n-3);
    my $ub = $z+1.96*$se;
    my $lb = $z-1.96*$se;
    $ub = (exp(2*$ub)-1)/(exp(2*$ub)+1);
    $lb = (exp(2*$lb)-1)/(exp(2*$lb)+1);
    return ($lb,$ub);
}
    

sub PrintPred {
    my ($name, $x, $y, $r2, $m, $b, $r2lb, $r2ub) = @_;
    print "$name:\n";
    print "    $y = $m * $x + $b  (R2=$r2 [$r2lb,$r2ub])\n";
}


<STDIN>;

$total=0;

while (<STDIN>) {
	chomp;
#	print STDERR;
	@x=split;
#	print STDERR " => $#x\n";
	$tnom[$total]=$x[2];
	$tlb[$total]=$x[3];
	$texp[$total]=$x[4];
	$tub[$total]=$x[5];
	$tact[$total]=$x[6];
	$tusr[$total]=$x[7];
	$relerr[$total] = ($texp[$total]-$tact[$total])/$texp[$total];
	$replayerr[$total] = ($tnom[$total]-$tusr[$total])/$tnom[$total];
	if ($tact[$total]>=$tlb[$total] && $tact[$total]<=$tub[$total]) {
		$inrange++;
	}
	$ciwidth[$total] = $tub[$total]-$tlb[$total];
	$ciwidthrelexp[$total] = $ciwidth[$total]/$texp[$total];
 	$ciwidthrelnom[$total] = $ciwidth[$total]/$tnom[$total];
 	$ciwidthrelact[$total] = $ciwidth[$total]/$tact[$total];
	if ($#x>8) { 
	    $tau[$total] = $x[9];
	}
	$total++;
}

$percentinrange = 100*$inrange/$total;
$relerravg = Mean(\@relerr);
$relerrstd = StdDev(\@relerr);
$replayerravg = Mean(\@replayerr);
$replayerrstd = StdDev(\@replayerr);

$ciwidthavg = Mean(\@ciwidth);
$ciwidthstd = StdDev(\@relerr);
$ciwidthrelexpavg = Mean(\@ciwidthrelexp);
$ciwidthrelexpstd = StdDev(\@ciwidthrelexp);
$ciwidthrelnomavg = Mean(\@ciwidthrelnom);
$ciwidthrelnomstd = StdDev(\@ciwidthrelnom);
$ciwidthrelactavg = Mean(\@ciwidthrelact);
$ciwidthrelactstd = StdDev(\@ciwidthrelact);



($replayr2,$replaym,$replayb) = LeastSquaresLine(\@tnom,\@tusr);
($relerrr2,$relerrm,$relerrb) = LeastSquaresLine(\@tnom,\@relerr);
($nompredr2,$nompredm,$nompredb) = LeastSquaresLine(\@tnom,\@tact);
($lbpredr2,$lbpredm,$lbpredb) = LeastSquaresLine(\@tlb,\@tact);
($exppredr2,$exppredm,$exppredb) = LeastSquaresLine(\@texp,\@tact);
($ubpredr2,$ubpredm,$ubpredb) = LeastSquaresLine(\@tub,\@tact);


($cirelexpr2,$cirelexpm,$cirelexpb) = LeastSquaresLine(\@texp,\@ciwidth);
($cirelnomr2,$cirelnomm,$cirelnomb) = LeastSquaresLine(\@tnom,\@ciwidth);
($cirelactr2,$cirelactm,$cirelactb) = LeastSquaresLine(\@tact,\@ciwidth);


@replayr2ci = r2ci($replayr2,$total);
@relerrr2ci = r2ci($relerrr2,$total);
@nompredr2ci = r2ci($nompredr2,$total);
@lbpredr2ci = r2ci($lbpredr2,$total);
@exppredr2ci = r2ci($exppredr2,$total);
@ubpredr2ci = r2ci($ubpredr2,$total);
@cirelexpr2ci = r2ci($cirelexpr2,$total);
@cirelnomr2ci = r2ci($cirelnomr2,$total);
@cirelactr2ci = r2ci($cirelactr2,$total);


if ($#tau>=0 && Variance(\@tau)>0) { 
    ($taur2,$taum,$taub) = LeastSquaresLine(\@tau,\@relerr);
    @taur2ci = r2ci($taur2,$total);
    $didtau=1;
} else {
    $didtau=0;
}


print "samples:                        $total\n";
print "average relative pred error:    $relerravg\t(std=$relerrstd)\n";
print "average replay error:           $replayerravg\t(std=($replayerrstd)\n";
print "percent in CI:                  $percentinrange %\n";
print "\n";
print "average CI width:               $ciwidthavg\t(std=$ciwidthstd)\n";
print "average CI width / texp:        $ciwidthrelexpavg\t(std=$ciwidthrelexpstd)\n";
print "average CI width / tnom:        $ciwidthrelnomavg\t(std=$ciwidthrelnomstd)\n";
print "average CI width / tact:        $ciwidthrelactavg\t(std=$ciwidthrelactstd)\n";
print "\n";
PrintPred("replay error","tnom","tusr",$replayr2,$replaym,$replayb,@replayr2ci);
PrintPred("relerr vs tnom","tnom","relerr",$relerrr2,$relerrm,$relerrb,@relerrr2ci);
if ($didtau) { 
    PrintPred("relerr vs tau","tau","relerr",$taur2,$taum,$taub,@taur2ci);
    $tauchoice = -$taub/$taum;
    print "Recommended tau value: $tauchoice\n";
}
print "\n";
PrintPred("nominal time predictor","tnom","tact",$nompredr2,$nompredm,$nompredb,@nompredr2ci); 
PrintPred("lb predictor","tlb","tact",$lbpredr2,$lbpredm,$lbpredb,@lbpredr2ci);
PrintPred("exp predictor","texp","tact",$exppredr2,$exppredm,$exppredb,@exppredr2ci);
PrintPred("ub predictor","tub","tact",$ubpredr2,$ubpredm,$ubpredb,@ubpredr2ci);
print "\n";
PrintPred("ciwidth vs texp","texp","ciwidth",$cirelexpr2,$cirelexpm,$cirelexpb,@cirelexpr2ci);
PrintPred("ciwidth vs tnom","tnom","ciwidth",$cirelnomr2,$cirelnomm,$cirelnomb,@cirelnomr2ci);
PrintPred("ciwidth vs tact","tact","ciwidth",$cirelactr2,$cirelactm,$cirelactb,@cirelactr2ci);








