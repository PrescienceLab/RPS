#!/usr/local/bin/perl5 -w

use Getopt::Long;

$usage = "get_predictions_on.pl [--bufferport=bufferport] host";

$bufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};

&GetOptions("bufferport=i"=>\$bufferport) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "predbufferclient 1 client:tcp:$host:$bufferport";

system $CMD;

