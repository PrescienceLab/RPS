#! /usr/bin/env perl

$usage = "measure_tsunami_perf.pl inputfile numtests\n";

$#ARGV==1 or die $usage;

$file = shift;;
$numtests = shift;

# STREAMING TESTS
# ---------------

# Max samplerate test, sample


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

system "rm perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
print STDERR "Sweeping BM from 1..512\n";
for ($p=1;$p<=512;$p*=2) {
    print STDERR "Perftesting BM $p\n";
    print STDERR "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out\n";
    system "perftest $file $file $numahead $numpasses $fitint $testint $flat BM $p >> perftest.BM.$numahead.$numpasses.$fitint.$testint.out";
}
