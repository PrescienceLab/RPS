package simple_stats;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(Sum Mean Variance StdDev Min LeastSquaresLine r2ci t_test_data t_test_sums t_test_stats);


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

sub GetColumn {
    my ($colno, $testref) = @_;
    my $row;
    my @col;
    foreach $row (@{$testref}) {
	push @col, @{$row}[$colno];
   }
   return @col;
}



sub ci_cmp {
    my ($min,$max) = @_;
    if (($min<0) && ($max<0)) {
	return -1;
    } elsif (($min<=0) && ($max>=0)) {
	return 0;
    } elsif (($min>0) && ($max>0)) {
	return +1;
    }
}
    

#
# 2-sample t-test given data array references
#
sub t_test_data {
  my ($aref, $bref,$conf) = @_;
  my ($na, $suma, $sum2a,$nb,$sumb,$sum2b);

  $na=$#{$aref}+1;
  $nb=$#{$bref}+1;

  $suma=0; $sum2a=0;
  for ($i=0;$i<$na;$i++) {
    $suma+=$aref->[$i];
    $sum2a+=($aref->[$i])**2;
  }

  $sumb=0; $sum2b=0;
  for ($i=0;$i<$nb;$i++) {
    $sumb+=$bref->[$i];
    $sum2b+=($bref->[$i])**2;
  }
  
#  print "$na\t$suma\t$sum2a\t$nb\t$sumb\t$sum2b\n";

  return t_test_sums($na,$suma,$sum2a,$nb,$sumb,$sum2b,$conf);
}

#
# 2-sample t-test given data counts, sums, and sum of squares
#
sub t_test_sums {
  my ($na, $suma, $sum2a,$nb,$sumb,$sum2b,$conf) = @_;

  $meana = $suma/$na;
  $meanb = $sumb/$nb;
  $stda = sqrt(($sum2a - $na*$meana**2)/($na-1));
  $stdb = sqrt(($sum2b - $nb*$meanb**2)/($nb-1));
  
#  print "$meana\t$stda\t$meanb\t$stdb\n";

  return t_test_stats($na,$meana,$stda,$nb,$meanb,$stdb,$conf);
}


  
  
# returns the $conf confidence interval of the mean difference
# for unpaired observations given the sums
#
# It is assumed that there are a sufficient number of dofs so that
# a normality assumption can be made
#
sub t_test_stats {
  my ($na, $meana, $stda, $nb, $meanb, $stdb, $conf) = @_;

  $meandiff = $meana - $meanb;

  $stddiff = sqrt($stda**2/$na + $stdb**2/$nb);
  
  $dof = (($stda**2/$na + $stdb**2/$nb)**2)
          /
         ((1/($na+1))*($stda**2/$na)**2 + (1/($nb+1))*($stdb**2/$nb)**2)  - 2;


#  print "$meandiff\t$stddiff\t$dof\n";

  if ($dof<30) {
    print STDERR "Warning - t_test_stats using normal with less than 30 effective dofs\n";
  }
  
  if ($conf!=0.95) {
    print STDERR "Warning - t_test_stats doing 95% conf interval instead of $conf*100%\n";
  }

  return ($meandiff-1.96*$stddiff,$meandiff+1.96*$stddiff);
}


