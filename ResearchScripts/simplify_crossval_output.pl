#!/usr/local/bin/perl5 -w
#
# find_diffs.pl [..] | simplify.pl

while (<STDIN>) {
    if (!/\#/) {
	($tag,$lead,$p,$d,$q,$sigmean,$sigsdev,$bmmean,$bmsdev,$msqmean,$msqsdev,$sigmsqstr,$sigbmstr,$msqbmstr) = split(/\s+/);
	$pimprove = 100.0*($bmmean-$msqmean)/$bmmean;
#	print "$lead\t$p\t$q\t$q\t$q\t$pimprove%\t$msqbmstr\n";
	print "$tag\t$lead\t$p\t$d\t$q\t$sigbmstr\t$sigmsqstr\t$msqbmstr\t$pimprove%\n";
    }
}
