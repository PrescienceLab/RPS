#! /usr/bin/env perl
use FileHandle;
use Getopt::Long;



&GetOptions(("matlab"=>\$matlab, "step"=>\$step));

$#ARGV>= 2 or die "usage: plot_impulse_resp.pl [--matlab] [--step] file length model\n";

$infile = shift; 
$resplen=shift;
$model = join(" ",@ARGV);

if ($step) {
  $CMD = "impulseresp $infile s$resplen $model >impulseresp.txt";
} else {
  $CMD = "impulseresp $infile $resplen $model >impulseresp.txt";
}

system "$CMD";

if ($matlab) { 
  open(MATLAB,"| matlab");
  MATLAB->autoflush(1);
  $cmds =<<DONE
load('impulseresp.txt');
x=impulseresp(:,3);
f=fft(x);
for i=1:length(f), pf(i)=phase(f(i)); end;
subplot(2,2,1);
plot(x);
xlabel('lag');
ylabel('resp');
title('impulse response');
xlim=get(gca,'XLim');
ylim=get(gca,'YLim');
text(xlim(1),ylim(2)+(ylim(2)-ylim(1))*0.1,'$infile $model');
subplot(2,2,3),plot(f,'o'),title('FFT of impulse response');
subplot(2,2,2),plot(abs(f(1:length(f)/2))),xlabel('freq'),ylabel('resp'),title('frequency response');
subplot(2,2,4),plot((180/pi)*pf(1:length(pf)/2)),xlabel('freq'),ylabel('degrees'),title('phase response');
DONE
;

  if ($step) { 
    $cmds =~ s/impulse response/step response/g;
  }

  print MATLAB $cmds;


  $finish = <STDIN>;
  if ($finish =~ /^p\s+(.*)/) {
    $printfile=$1;
    print "Saving to $printfile\n";
    print MATLAB "print -deps '$printfile';\n";
  }
  print MATLAB "quit\n";
  close(MATLAB);
} else {
  open(GNUPLOT,"| gnuplot");
  GNUPLOT->autoflush(1);
  $cmds = <<DONE2
set title "impulse response of $infile $model
plot "impulseresp.txt" using 1:3 with linespoints
DONE2
;
  if ($step) {
    $cmds =~ s/impulse response/step response/g;
  }

  print GNUPLOT $cmds;

  $finish = <STDIN>;
  if ($finish =~ /^p\s+(.*)/) {
    $printfile=$1;
    print "Saving to $printfile\n";
print GNUPLOT <<DONE3
set terminal postscript eps color
set output "$printfile"
$cmds
DONE3
;
  }
  close(GNUPLOT);
}


#print MATLAB "subplot(2,2,2),loglog(abs(f(1:length(f)/2)),xlabel('log freq'),ylabel('log resp'),title('frequency response');\n";
#print MATLAB "subplot(2,2,4),semilogx((180/pi)*pf(1:length(pf)/2)),xlabel('log freq'),ylabel('degrees'),title('phase response');\n";



exit;
