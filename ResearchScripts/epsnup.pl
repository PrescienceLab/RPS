#!/usr/bin/env perl

use Getopt::Long;

$width= 6;  # inches

$cols = 2;
$outfile="epsnup.ps";

&GetOptions("cols=i"=>\$cols, "outfile=s"=>\$outfile) and $#ARGV >=0 or die "usage: epsnup.pl [--cols=cols] [--outfile=outfile] epsfiles";

@files = @ARGV;
while (($#files+1)%$cols) {
    push @files, "EMPTY";
}
$numfiles = $#files;

$figwidth=$width/$cols;

open(TEX,">_epsnup.tex");
print TEX "\\documentclass{report}\n\\usepackage{fullpage}\n\\usepackage{epsf}\n\\usepackage{times}\n\\usepackage{longtable}\n\n\\begin{document}\n";

print TEX "\\begin{longtable}{";
for ($i=0;$i<$cols;$i++) { 
    print TEX "c";
}
print TEX "}\n";

for ($row=0;$row<$numfiles/$cols;$row++) { 
    for ($col=0;$col<$cols;$col++) {
	if (!($files[$row*$cols+$col] eq "EMPTY")) { 
	    print TEX "\\epsfxsize=${figwidth}in \\epsfbox{$files[$row*$cols+$col]} ";
	} else {
	    print TEX "EMPTY";
	}
	if ($col==($cols-1)) { 
	    print TEX "\\\\\n";
	} else {
	    print TEX "&";
	}
    }
}

print TEX "\\end{longtable}\n\n\\end{document}\n";
close(TEX);

system "latex2e _epsnup";
system "dvips -f _epsnup.dvi -o $outfile";
system "rm -f _epsnup.*";


