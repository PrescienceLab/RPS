#! /usr/bin/env perl

$usage = "measure_tsunami_perf.pl inputfile numtests\n";

$#ARGV==1 or die $usage;

$file = shift;;
$numtests = shift;

# STREAMING TESTS
# ---------------

# Max samplerate test, sample
system "rm perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping BM from 1..512\n";
for ($p=1;$p<=512;$p*=2) {
    print STDERR "Perftesting BM $p\n";
    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out\n";
    system "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
}

# Max samplerate test, block


# Streaming scalability test as levels added, sample, forward

# Streaming scalability as wavelet type order increases, sample, forward

# Streaming scalability test as levels added, block, forward

# Streaming scalability as wavelet type order increases, block, forward


# Streaming scalability test as levels added, sample, delay + reverse

# Streaming scalability as wavelet type order increases, sample, delay + reverse

# Streaming scalability test as levels added, block, delay + reverse

# Streaming scalability as wavelet type order increases, block, delay + reverse


# DISCRETE TESTS
# ---------------

# Max sample rate test, forward transform

# Max sample rate test, reverse transform

# Discrete forward scalability test as blocksize increases -> levels increase

# Discrete forward scalability test as wavelet type increases order, blocksize constant

# Discrete reverse scalability test as blocksize increases -> levels increase

# Discrete reverse scalability test as wavelet type increases order, blocksize constant

# mean model
print STDERR "Perftesting MEAN\n";
print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat MEAN > perftest.MEAN.$numahead.$numpasses.$fitint.$testint.out\n";
system "perftest $file $file $numahead $numpasses $fitint $testint $flat MEAN > perftest.MEAN.$numahead.$numpasses.$fitint.$testint.out";


# last model
print STDERR "Perftesting LAST\n";
print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat LAST > perftest.LAST.$numahead.$numpasses.$fitint.$testint.out\n";
system "perftest $file $file $numahead $numpasses $fitint $testint $flat LAST > perftest.LAST.$numahead.$numpasses.$fitint.$testint.out";


# sweep best mean from 1..512
system "rm perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping BM from 1..512\n";
for ($p=1;$p<=512;$p*=2) {
    print STDERR "Perftesting BM $p\n";
    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out\n";
    system "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
}

# sweep AR from 1..512
system "rm perftest.AR.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping AR from 1..512\n";
for ($p=1;$p<=512;$p*=2) {
    print STDERR "Perftesting AR $p\n";
    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat AR $p >> perftest.AR.$numahead.$numpasses.$fitint.$testint.out\n";
    system "perftest $file $file $numahead $numpasses $fitint $testint $flat AR $p >> perftest.AR.$numahead.$numpasses.$fitint.$testint.out";
}

# sweep MA from 1..16
system "rm perftest.MA.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping MA from 1..16\n";
for ($q=1;$q<=16;$q*=2) {
    print STDERR "Perftesting MA $q\n";
    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat MA $q >> perftest.MA.$numahead.$numpasses.$fitint.$testint.out\n";
    system "perftest $file $file $numahead $numpasses $fitint $testint $flat MA $q >> perftest.MA.$numahead.$numpasses.$fitint.$testint.out";
}

# sweep ARMA from 1..8, 1..8
system "rm perftest.ARMA.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping ARMA from 1..8, 1..8\n";
for ($p=1;$p<=8;$p*=2) {
    for ($q=1;$q<=8;$q*=2) {
	print STDERR "Perftesting ARMA $p $q\n";
	print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat ARMA $p $q >> perftest.ARMA.$numahead.$numpasses.$fitint.$testint.out\n";
	system "perftest $file $file $numahead $numpasses $fitint $testint $flat ARMA $p $q >> perftest.ARMA.$numahead.$numpasses.$fitint.$testint.out";
    }
}

# sweep ARIMA from 1..8, 1..3, 1..8
system "rm perftest.ARIMA.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping ARIMA from 1..8, 1..3, 1..8\n";
for ($p=1;$p<=8;$p*=2) {
    for ($d=1;$d<=3;$d++) { 
	for ($q=1;$q<=8;$q*=2) {
	    print STDERR "Perftesting ARIMA $p $d $q\n";
	    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat ARIMA $p $d $q >> perftest.ARIMA.$numahead.$numpasses.$fitint.$testint.out\n";
	    system "perftest $file $file $numahead $numpasses $fitint $testint $flat ARIMA $p $d $q >> perftest.ARIMA.$numahead.$numpasses.$fitint.$testint.out";
	}
    }
}


# sweep ARFIMA from 1..8, -1, 1..8
system "rm perftest.ARFIMA.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping ARMA from 1..8, -1, 1..8\n";
for ($p=1;$p<=8;$p*=2) {
    for ($q=1;$q<=8;$q*=2) {
	print STDERR "Perftesting ARFIMA $p -1 $q\n";
	print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat ARFIMA $p -1 $q >> perftest.ARFIMA.$numahead.$numpasses.$fitint.$testint.out\n";
	system "perftest $file $file $numahead $numpasses $fitint $testint $flat ARFIMA $p -1 $q >> perftest.ARFIMA.$numahead.$numpasses.$fitint.$testint.out";
    }
}
    
