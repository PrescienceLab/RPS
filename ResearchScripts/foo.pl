#! /usr/bin/env perl

use FileHandle;

autoflush STDOUT 1;

open(VMSTAT,"vmstat 1 |");

while (<VMSTAT>) {
    chomp;
    if (/r/) {
	next;
    }
    @f=split; 
    $n=$#f;
    $us=$f[$n-2]; $sy=$f[$n-1]; $tot=$us+$sy ;
    print "$us\t$sy\t$tot\n";
}

