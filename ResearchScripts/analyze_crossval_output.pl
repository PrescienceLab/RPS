#!/usr/local/bin/perl5 -w
#
# find_diffs.pl [conf] < output from crossval_generic
#
# Scan through output from crossval_generic
# and for each lead and model, compute
# mean+stddev of err for the model msq, the bm model msq and the signal variance
# the perform t-test to see if model and bm model are different from the sigal and
# different from each other with the supplied confidence
#
#


if ($#ARGV<0) {
    $conf=0.95;
} else {
    print STDERR "Only know how to do 0.95 for now...\n";
    $conf=0.95;
#    $conf=$ARGV[0];
}

while (<STDIN>) {
    if (!/#/) {
	@items = split(/\s+/);
	$tag=$items[0];
	$lead=$items[2];
	$p=$items[11];
	$d=$items[12];
	$q=$items[13];
	push @{$sig{$tag}{$lead}{$p}{$d}{$q}}, $items[10];
	push @{$msq{$tag}{$lead}{$p}{$d}{$q}}, $items[21];
	push @{$bmmsq{$tag}{$lead}{$p}{$d}{$q}}, $items[16];
    }
}


print "#tag\tlead\tp\td\tq\tsigmean\tsigsdev\tbmmean\tbmsdev\tmsqmean\tmsqsdev\tsigmsqstr\tsigbmstr\tmsqbmstr\n";
foreach $tag (sort keys %sig) {
    foreach $lead (sort numerically keys %{$sig{$tag}}) {
	foreach $p (sort numerically keys %{$sig{$tag}{$lead}}) {
	    foreach $d (sort numerically keys %{$sig{$tag}{$lead}{$p}}) {
		foreach $q (sort numerically keys %{$sig{$tag}{$lead}{$p}{$d}}) {
		    $sigmean = Mean(@{$sig{$tag}{$lead}{$p}{$d}{$q}});
		    $sigsdev = StdDev(@{$sig{$tag}{$lead}{$p}{$d}{$q}});
		    $msqmean = Mean(@{$msq{$tag}{$lead}{$p}{$d}{$q}});
		    $msqsdev = StdDev(@{$msq{$tag}{$lead}{$p}{$d}{$q}});
		    $bmmean = Mean(@{$bmmsq{$tag}{$lead}{$p}{$d}{$q}});
		    $bmsdev = StdDev(@{$bmmsq{$tag}{$lead}{$p}{$d}{$q}});
		    
		    $sigmsqdiff = ClassifyConfInterval(ConfIntervalForMeanDiff(\@{$sig{$tag}{$lead}{$p}{$d}{$q}},\@{$msq{$tag}{$lead}{$p}{$d}{$q}},$conf));
		    $sigbmdiff = ClassifyConfInterval(ConfIntervalForMeanDiff(\@{$sig{$tag}{$lead}{$p}{$d}{$q}},\@{$bmmsq{$tag}{$lead}{$p}{$d}{$q}},$conf));
		    $bmmsqdiff = ClassifyConfInterval(ConfIntervalForMeanDiff(\@{$bmmsq{$tag}{$lead}{$p}{$d}{$q}},\@{$msq{$tag}{$lead}{$p}{$d}{$q}},$conf));
		    
		    $sigmsqstr=ConfClassToString($sigmsqdiff,$conf);
		    $sigbmstr=ConfClassToString($sigbmdiff,$conf);
		    $msqbmstr=ConfClassToString($bmmsqdiff,$conf);
		    print "$tag\t$lead\t$p\t$d\t$q\t$sigmean\t$sigsdev\t$bmmean\t$bmsdev\t$msqmean\t$msqsdev\t$sigmsqstr\t$sigbmstr\t$msqbmstr\n";
		}
	    }
	}
    }
}


sub ConfClassToString {
    my ($test,$conf)=@_;
    if ($test<0) {
	return "WORSE($conf)";
    }
    if ($test>0) {
	return "BETTER($conf)";
    }
    if ($test==0) {
	return "same($conf)";
    }
}

#
# NOTE:  Large sample case (>30 samples)
#
sub ConfIntervalForMean {
    my ($sampleref, $conflevel) = @_;

    if ($conflevel!=0.95) {
	print STDERR "Don't know how to deal with conflevel=$conflevel yet!\n";
	return (-1000,1000);
    }
    
    $n = $#{@$sampleref};
    if ($n<=30) {
	print STDERR "Warning: Large sample conf interval on <=30 samples\n";
    }
    $mean = Mean(@$sampleref);
    $sdev = StdDev(@$sampleref);
    
    return ($mean-1.96*$sdev/sqrt($n),$mean+1.96*$sdev/sqrt($n));
}

sub ConfIntervalForMeanDiff {
    my ($leftref, $rightref, $conf) = @_;
    
    @diffs = Diff($leftref,$rightref);

    return ConfIntervalForMean(\@diffs,$conf);
}


sub ClassifyConfInterval {
    my ($left, $right) = @_;
    if (RangeIncludesZero($left,$right)) {
	return 0;
    }
    if (RangeGreaterThanZero($left,$right)) {
	return +1;
    }
    if (RangeLessThanZero($left,$right)) {
	return -1;
    }
}


sub RangeIncludesZero {
    my ($left, $right) = @_;

    if ($left<=0 && $right>=0) {
	return 1;
    } else {
	return 0;
    }
}

sub RangeGreaterThanZero {
    my ($left, $right) = @_;

    if ($left>0 && $right>0) {
	return 1;
    } else {
	return 0;
    }
}

sub RangeLessThanZero {
    my ($left, $right) = @_;

    if ($left<0 && $right<0) {
	return 1;
    } else {
	return 0;
    }
}


sub SignificantlySmaller {
    my ($leftref, $rightref, $conf) = @_;
    
    @diffs = Diff($leftref,$rightref);

    ($low,$high) = ConfIntervalForMean(\@diffs,$conf);
    
    return RangeLessThanZero($low,$high);
}

sub SignificantlyLarger {
    my ($leftref, $rightref, $conf) = @_;
    
    @diffs = Diff($leftref,$rightref);

    ($low,$high) = ConfIntervalForMean(\@diffs,$conf);
    
    return RangeGreaterThanZero($low,$high);
}


   

    
    

sub Mean {
    my $i;
    my $sum=0;
    for ($i=0;$i<=$#_;$i++) {
	$sum+=$_[$i];
    }
    return $sum/($#_+1);
}

sub Variance {
    my $i;
    my $sum=0;
    my $mean = Mean(@_);
    for ($i=0;$i<=$#_;$i++) {
	$sum+=($_[$i]-$mean)**2;
    }
    return $sum/($#_);
}

sub StdDev {
    return sqrt(Variance(@_));
}


sub Diff {
    my ($leftref,$rightref) = @_;
    my @outlist;
    my $i;
    my $num = max($#{@$leftref},$#{@$rightref});

#    print STDERR "In Diff\n";
    for ($i=0;$i<$num;$i++) {
#	print STDERR "differencing $leftref->[$i] and $rightref->[$i]\n";
	push @outlist, $leftref->[$i] - $rightref->[$i];
    }
    return @outlist;
}


sub max {
    if ($#_<0) {
	print STDERR "max called on empty list\n";
	return 0;
    } else {
	my $curm=$_[0];
	my $i;
	for ($i=1;$i<$#_+1;$i++) {
	    if ($_[$i]>$curm) { 
		$curm=$_[$i];
	    }
	}
	return $curm;
    }
}

sub numerically { $a <=> $b }

    
	 
