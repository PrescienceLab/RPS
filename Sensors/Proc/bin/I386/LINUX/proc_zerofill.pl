#!/usr/bin/perl -w

$#ARGV==1 or die "usage: proc_zerofill.pl infile outfile\n";

$infile=shift;
$outfile=shift;

open(IN,$infile);
open(OUT,">$outfile");

@lines=<IN>;
close(IN);

$max=-1;

foreach $line (@lines) { 
    chomp $line;
    @nums=split /\s+/, $line;
    if ($#nums > $max) { $max=$#nums } 
    push @out, \@nums;
}


foreach $item (@out) { 
    $count = $#{$item};
    print OUT join("\t",@{$item});
    for ($i=0;$i<$max-$count;$i++) { print OUT "\t0";}
    print OUT "\n";
}

close(OUT)


    
