#!/usr/bin/env perl
use RPS;

use Getopt::Long;


$usage = "Get wavelet coefficients from a wavelet buffer\n\n".
"usage: get_wavelet_measurements_on.pl [--waveletbufferport=port] [--num=number] host\n".RPSBanner();

$bufferport = $ENV{"WAVELETBUFFERPORT"};

$num = 1024;

&GetOptions(("waveletbufferport=i"=>\$bufferport, "num=i"=>\$num) ) ;

$#ARGV==0 and $host=$ARGV[0] or die $usage;

open(W,"wavelet_bufferclient $num client:tcp:$host:$bufferport |");

while (<W>) { 
  chomp;
  $next=<W>;
  print $_.$next;
}

close(W);

