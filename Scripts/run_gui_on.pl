#!/usr/bin/env perl

use RPS;
use Getopt::Long;

$usage = 
"Run heads-up display on a sensor, predictor, or both\n\n".
"usage: run_gui_on.pl [--measurebufferport=port] [--useloadmeasure]  [--nummeasure=num]\n".
"                     [--predbufferport=port] [--numpred=num] [--rate=Hz] [--noconf]\n".
"                     [--nopred] [--nomeasure] [--help] host\n".
RPSBanner();

$nopred=0;
$nomeasure=0;
$measurebufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$loadbufferport = $ENV{"HOSTLOADSERVERBUFFERPORT"};
$predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};
$nummeasure=30;
$numpred=30;
$noconf=0;
$rate=1.0;
$useloadmeasure=0;

&GetOptions("measurebufferport=i"=>\$measurebufferport,
	    "loadbufferport=i"=>\$loadbufferport, 
            "predbufferport=i"=>\$predbufferport,
	    "nummeasure=i"=>\$nummeasure,
	    "numpred=i"=>\$numpred,
	    "noconf"=>\$noconf,
	    "rate=f"=>\$rate,
            "nopred"=>\$nopred,
            "nomeasure"=>\$nomeasure,
	    "help"=>\$help,
	    "useloadmeasure"=>\$useloadmeasure) ;

if ($help || ($nomeasure && $nopred) ) { 
  print $usage;
  exit(0);
}

$#ARGV==0 and $host=$ARGV[0] or die $usage;

$mode="";
$line=" $rate ";
if (!$nomeasure) { 
  if ($useloadmeasure) { 
    $mode.="l";
    $line.=" client:tcp:$host:$loadbufferport $nummeasure ";
  } else {
    $mode.="m";
    $line.=" client:tcp:$host:$measurebufferport $nummeasure ";
  }
} 

if (!$nopred) { 
  $mode.="p";
  $line.=" client:tcp:$host:$predbufferport $numpred ";
  if (!$noconf) { 
    $line.=" conf ";
  }
}

$line = "$mode $line";

$CMD = "(export CLASSPATH=.; export LD_LIBRARY_PATH=.; cd \$RPS_DIR/JavaGUI; ./show_pred.sh $line ) & ";

#print "$CMD\n";

system $CMD;
