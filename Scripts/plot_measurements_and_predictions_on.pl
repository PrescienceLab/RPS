#!/usr/bin/env perl
use RPS;
use POSIX;
use FileHandle;
use Getopt::Long;

$usage = "Plot measurements and predictions using gnuplot\n\n".
"usage: plot_measurements_and_predictions_on.pl [--measurebufferport=measurebufferport]\n".
"        [--nummeasure=num] [--predbufferport=predbufferport] [--numpred=num]\n".
"        [--rate=Hz] [--nomeasure] [--nopred] host\n".RPSBanner();


$measurebufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};
$nummeasure=1000;
$numpred=1000;
$nomeasure=0;
$nopred=0;
$rate=-1;

&GetOptions(("measurebufferport=i"=>\$measurebufferport,
	     "predbufferport=i"=>\$predbufferport,
	     "nummeasure=i"=>\$nummeasure,
	     "numpred=i"=>\$numpred,
	     "nomeasure"=>\$nomeasure,
	     "nopred"=>\$nopred,
	     "rate=f"=>\$rate)) ;

$#ARGV==0 and $host=$ARGV[0] and (not ($nopred==1 && $nomeasure==1)) or die $usage;


open (GNUPLOT,"|gnuplot");
GNUPLOT->autoflush(1);


$mfile="_temp.".getpid();
$pfile="_tempp.".getpid();

$SIG{INT} = sub { system "rm -f $mfile $pfile"; exit(0)} ;


do { 


  $start=0;

  if (!$nomeasure) { 
    $CMD = "get_measurements_on.pl --measurebufferport=$measurebufferport --num=$nummeasure $host |";
    
    open(IN,"$CMD");
    open(OUT,">$mfile");
    while (<IN>) {
      chomp;
      split;
      if ($start==0) { 
	$start=$_[1];
      }
      $time=$_[1]-$start;
      print OUT "$time\t$_[2]\n";
    }
    close(IN);
    close(OUT);
  }

  if (!$nopred) { 
    
    $CMD = "get_predictions_on.pl --predbufferport=$predbufferport $host |";

#print $CMD;

    
    open(IN,"$CMD");
    open(OUT,">$pfile");
    $line=<IN>;
    $line=~/.*F\((.*)Hz\).*M\((.*)secs\).*human-name=\'(.*)\'/;
    $period=1.0/$1;
    $timestart=$2;
    $model=$3;
    $i=0;
    while (<IN>) {
      leave if ($i>=$numpred);
#      print;
      chomp;
      split;
      $time=$_[0]*$period+$timestart;
      $value=$_[1];
      if ($start==0) { 
	$start=$time;
      }
      $time-=$start;
      print OUT "$time\t$value\n";
      $i++;
    }
    close(IN);
    close(OUT);
  }
  


  $cmds="set title \"";
  if (!$nomeasure) { 
    $cmds.="Measurements on $host:$measurebufferport\\n";
  }
  if (!$nopred) { 
    $cmds.="Predictions on $host:$predbufferport\\nperiod:$period predstart=$timestart model=$model";
  }
 $cmds.="\\nstarttime=$start\"\nset xlabel 'Time'\nset ylabel 'Value'\n";

  if (!$nomeasure) { 
    $cmds.= "plot '$mfile' using 1:2 title 'Measurements' with linespoints";
    if (!$nopred) { 
      $cmds.=", ";
    }
  } else {
    $cmds.= "plot ";
  }
  if (!$nopred) { 
    $cmds.= "'$pfile' using 1:2 title 'Predictions' with linespoints";
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

system "rm -f $mfile $pfile";
