#!/usr/bin/env perl

use Getopt::Long;

$usage = "get_measurements_on.pl [--bufferport=bufferport] host";

$bufferport = $ENV{"HOSTLOADMEASUREBUFFERPORT"};

&GetOptions("bufferport=i"=>\$bufferport) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "measurebufferclient 1 client:tcp:$host:$bufferport";

system $CMD;

