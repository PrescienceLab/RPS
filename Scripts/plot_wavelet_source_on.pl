#!/usr/bin/env perl
use RPS;
use POSIX;
use FileHandle;
use Getopt::Long;

$usage = "Plot wavelet-transformed measurements using gnuplot\n\n".
"usage: plot_wavelet_source_on.pl [--waveletbufferport=port]\n".
"        [--rate=Hz]  [--numcoeffs=num] host\n".RPSBanner();


$waveletbufferport=$ENV{"WAVELETBUFFERPORT"};
$numcoeffs=1024;
$rate=-1;

&GetOptions(("waveletbufferport=i"=>\$waveletbufferport,"numcoeffs=i"=>\$numcoeffs,
	     "rate=f"=>\$rate)) ;

$#ARGV==0 and $host=$ARGV[0] or die $usage;


open (GNUPLOT,"|gnuplot");
GNUPLOT->autoflush(1);


$mfile="_tempw.".getpid();

$SIG{INT} = sub { system "rm -f $mfile"; exit(0)} ;


do { 
  $first=0;

  $CMD = "get_wavelet_measurements_on.pl --waveletbufferport=$waveletbufferport --num=$numcoeffs $host |";
  
  open(IN,"$CMD");
  $min=99999999999999999999999999;
  $max=-99999999999999999999999999;
  @values=();
  @times=();
  @indices=();
  while (<IN>) {
    /timestamp=(.*), rinfo.*levels=(\d+).*index=(\d+),.*level=(\d+),.*value=(.*)/;
    $time=$1; $levels=$2; $index=$3; $level=$4; $value=$5;
    push @{$values[$level]}, $value;
    push @{$times[$level]}, $time;
    push @{$indices[$level]}, $index;
    if ($value<$min) { 
      $min=$value;
    }
    if ($value>$max) { 
      $max=$value;
    }
  }
  close(IN);

  $range=$max-$min;

  for ($i=0;$i<$levels;$i++) { 
    $first=$indices[$i][0];
    for ($j=0;$j<=$#{$indices[$i]};$j++) { 
#      print "$i\t$j";
      $indices[$i][$j]-=$first;
      $indices[$i][$j]*=2**$i;
      
      $values[$i][$j]+=$range*$i;
#      print "\t$indices[$i][$j]\n";
#      print "$i\t$indices[$i][$j]\t$values[$i][$j]\n";
    }
  }

  open(OUT,">$mfile");
  for ($i=0;$i<=$#{$values[0]};$i++) { 
    for ($j=0;$j<$levels;$j++) { 
      if ($j>0) { 
	print OUT "\t";
      }
      if ($i<=$#{$values[$j]}) { 
	print OUT "$indices[$j][$i]\t$values[$j][$i]";
      } else {
	print OUT "\t";
      }
    }
    print OUT "\n";
  }
  close(OUT);


  $cmds="set title \"Wavelet Coeffients on  $host:$waveletbufferport\\nlevels=$levels num=$numcoeffs\"\n";
  $cmds.="set xlabel 'Index'\nset ylabel 'Value'\n";
  $cmds.="plot ";

  for ($i=0;$i<=$levels;$i++) { 
    if ($i>0) { 
      $cmds.=", ";
    }
    $indexcol=$i*2+1;
    $valcol=$i*2+2;
    $cmds.= "'$mfile' using $indexcol:$valcol title 'Level $i' with linespoints";
  }
  $cmds.="\n";

  #print $cmds;

  print GNUPLOT $cmds;
  
  if ($rate<0) { 
    print "<enter> to exit, p file<enter> to print\n";
    STDIN->autoflush(1);
    $finish = <STDIN>;
    if ($finish =~ /^p\s+(.*)/) {
      $printfile=$1;
      print "Saving to $printfile\n";
      print GNUPLOT <<DONE2
set terminal postscript eps color
set output "$printfile"
$cmds
DONE2
	;
    }
  } else {
    select(undef,undef,undef,1.0/$rate)
  }
} while ($rate>0);

close(GNUPLOT);

system "rm -f $mfile";
