#! /usr/local/bin/perl5 -w
use FileHandle;

$infile = $ARGV[0];
$resplen=$ARGV[1];
@args = @ARGV[2 .. $#ARGV];


$CMD = "impulseresp $infile s$resplen";
$MODELJUNK="";
foreach $arg (@args) { 
    $CMD.=" $arg";
    $MODELJUNK.=" $arg";
}

print STDERR "$CMD > stepresp.txt\n";
system "$CMD > stepresp.txt";




open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
print GNUPLOT "set title \"step resp of $infile $MODELJUNK\"\n";
print GNUPLOT "plot \"stepresp.txt\" using 1:3 with linespoints\n";
$finish = <STDIN>;
if ($finish =~ /^p\s+(.*)/) {
    $printfile=$1;
    print "Saving to $printfile\n";
    print GNUPLOT "set terminal postscript\n";
    print GNUPLOT "set output \"$printfile\"\n";
    print GNUPLOT "set title \"Step Resp of $infile $MODELJUNK\"\n";
    print GNUPLOT "plot \"stepresp.txt\" using 1:3 with linespoints\n";
}
close(GNUPLOT);




exit;
