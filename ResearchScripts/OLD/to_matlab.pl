#!/usr/bin/env perl

while (<STDIN>) {
    ($timestamp, $host, $tnom, $tlb, $texp, $tub, $tact, $tusr, $tsys) = split;
    print "$timestamp\t$tnom\t$tlb\t$texp\t$tub\t$tact\t$tusr\t$tsys\n";
}


