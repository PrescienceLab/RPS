#!/usr/bin/perl -w


use CGI;
use POSIX;

$ENV{"RPS_DIR"} or die "set RPS_DIR\n";

$unique=getpid().rand();
$unique=~s/\s+//g;

$dir=$ENV{"RPS_DIR"}."/Web/Temp";

$infile="$dir/$unique.in";
$outfile="$dir/$unique.out";
$giffile="$dir/$unique.gif";

$query=new CGI;

$filename=$query->param("inputfile");
$fitfirst=$query->param("fitfirst");
$fitnum=$query->param("fitnum");
$testnum=$query->param("testnum");
$numahead=$query->param("numahead");
$model=$query->param("model");

$fh = $query->upload("inputfile");
open(UPLOAD,">$infile");
binmode(UPLOAD);
while (<$fh>) { print UPLOAD; }
close(UPLOAD);

print "Filename: $filename\n";
print "Fitfirst: $fitfirst\n";
print "Fitnum:   $fitnum\n";
print "Testnum:  $testnum\n";
print "Numahead: $numahead\n";
print "Model:    $model\n";

open(INPUT,"$infile");
while (<INPUT>) { print;}
close(INPUT);
