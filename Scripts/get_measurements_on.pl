#!/usr/bin/env perl
use RPS;

use Getopt::Long;


$usage = "Get measurements from a measure buffer\n\n".
"usage: get_measurements_on.pl [--measurebufferport=port] [--num=number] host\n".RPSBanner();

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};

$num = 1;

&GetOptions(("measurebufferport=i"=>\$bufferport, "num=i"=>\$num) ) ;

$#ARGV==0 and $host=$ARGV[0] or die $usage;

$CMD = "measurebufferclient $num client:tcp:$host:$bufferport";

system $CMD;

