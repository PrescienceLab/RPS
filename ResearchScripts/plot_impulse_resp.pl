#! /usr/bin/env perl
use FileHandle;

$infile = $ARGV[0];
$resplen=$ARGV[1];
@args = @ARGV[2 .. $#ARGV];


$CMD = "impulseresp $infile $resplen";
$MODELJUNK="";
foreach $arg (@args) { 
    $CMD.=" $arg";
    $MODELJUNK.=" $arg";
}

print STDERR "$CMD > impulseresp.txt\n";
system "$CMD > impulseresp.txt";



open(MATLAB,"| matlab");
MATLAB->autoflush(1);
print MATLAB "load('impulseresp.txt');\n";
print MATLAB "x=impulseresp(:,3);\n f=fft(x);\n";
print MATLAB "for i=1:length(f),pf(i)=phase(f(i)); end\n";
print MATLAB "subplot(2,2,1),plot(x),xlabel('lag'),ylabel('resp'),title('impulse response');\n";
print MATLAB "xlim=get(gca,'XLim');ylim=get(gca,'YLim');\n";
print MATLAB "text(xlim(1),ylim(2)+(ylim(2)-ylim(1))*0.1,'$infile $MODELJUNK');\n";
print MATLAB "subplot(2,2,3),plot(f,'o'),title('FFT of impulse response');\n";
print MATLAB "subplot(2,2,2),plot(abs(f(1:length(f)/2))),xlabel('freq'),ylabel('resp'),title('frequency response');\n";
print MATLAB "subplot(2,2,4),plot((180/pi)*pf(1:length(pf)/2)),xlabel('freq'),ylabel('degrees'),title('phase response');\n";
#print MATLAB "subplot(2,2,2),loglog(abs(f(1:length(f)/2)),xlabel('log freq'),ylabel('log resp'),title('frequency response');\n";
#print MATLAB "subplot(2,2,4),semilogx((180/pi)*pf(1:length(pf)/2)),xlabel('log freq'),ylabel('degrees'),title('phase response');\n";


$finish = <STDIN>;
if ($finish =~ /^p\s+(.*)/) {
    $printfile=$1;
    print "Saving to $printfile\n";
    print MATLAB "print -deps '$printfile';\n";
}
print MATLAB "quit\n";
close(MATLAB);



if (0) { 
open(GNUPLOT,"| gnuplot");
GNUPLOT->autoflush(1);
print GNUPLOT "set title \"impulse resp of $infile $MODELJUNK\"\n";
print GNUPLOT "plot \"_impulseresp.txt\" using 1:3 with linespoints\n";
$finish = <STDIN>;
close(GNUPLOT);
}



exit;
