#!/usr/bin/env perl
use RPS;
use Getopt::Long;

$usage = "Get predictions from a prediction buffer\n\n".
"usage: get_predictions_on.pl [--predbufferport=port] host\n".RPSBanner();

$bufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};

&GetOptions("predbufferport=i"=>\$bufferport) ;

$#ARGV==0 and $host=$ARGV[0] or die $usage;

$CMD = "predbufferclient 1 client:tcp:$host:$bufferport";

system $CMD;

