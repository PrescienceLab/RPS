#!/usr/bin/env perl

$#ARGV==0 or die "usage: windowsifyfile.pl file\n";

fixfile($ARGV[0],"\n","\r\n");

sub fixfile {
  my ($infile, $searchre, $replre) = @_;
  my $outfile=$infile."re";
#  print "$infile, $searchre, $replre\n";
  open (IN, $infile);
  open (OUT, ">$outfile");
  while (<IN>) {
    s/$searchre/$replre/g;
    print OUT;
  }
  close(IN);
  close(OUT);
  `mv $outfile $infile`;
}

