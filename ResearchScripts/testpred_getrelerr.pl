#!/usr/local/bin/perl5 -w

<STDIN>;

while (<STDIN>) {
	chomp;
	@x=split;
	$tnom=$x[2];
	$tlb=$x[3];
	$texp=$x[4];
	$tub=$x[5];
	$tact=$x[6];
	$tusr=$x[7];
	$relerr = ($texp-$tact)/$texp;
	$relusrerr = ($tnom-$tusr)/$tnom;
	if ($#x>8) { 
	    print "$tnom\t$tlb\t$texp\t$tub\t$tact\t$relerr\t$relusrerr\t$x[9]\n";
	} else {
	    print "$tnom\t$tlb\t$texp\t$tub\t$tact\t$relerr\t$relusrerr\n";
	}
}

