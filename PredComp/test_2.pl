#!/usr/bin/env perl

$waveletstream=5002;
$outputmeasurest=6003;
$outputmeasurebuf=6004;


$cmd = "bin/I386/LINUX/wavelet_streaming_client reconstruct source:tcp:localhost:$waveletstream | text2measure 100000 source:stdio:stdin connect:tcp:$outputmeasurest target:stdio:stdout | measurebuffer 10000 source:stdio:stdin server:tcp:$outputmeasurebuf";

print "$cmd\n";

system $cmd;

