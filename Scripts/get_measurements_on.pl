#!/usr/bin/env perl

use Getopt::Long;

$usage = "get_measurements_on.pl [--bufferport=bufferport] [--num=number] host";

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};
$num = 1;

&GetOptions(("bufferport=i"=>\$bufferport, "num=i"=>\$num) ) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "measurebufferclient $num client:tcp:$host:$bufferport";

system $CMD;

