#!/usr/bin/env perl

$#ARGV==0 or die "wt_usercpu.pl period_ms\n";

$period=shift;

system "wt_wrapper.pl $period \"\\PhysicalDisk(_Total)\\Disk Bytes/sec\"";


