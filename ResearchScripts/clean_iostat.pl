#! /usr/local/bin/perl5 -w

while (<STDIN>) { 
    if (/r/) {
	next;
    }
    @f=split; 
    $us=$f[14]; $sy=$f[15]; $tot=$us+$sy ;
    print "$us\t$sy\t$tot\n";
}

