#!/usr/bin/env perl

print "Fixing up RPS.dsw and .dsp files.\n";

fixfile("RPS.dsw","\n","\r\n");
foreach $dsp (split(/\s+/,`find . -name "*.dsp" -print`)) {
  print "Fixing $dsp\n";
  fixfile($dsp,"\n","\r\n");
}


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

