#!/usr/bin/env perl

$inputmeasurest=5000;
$inputmeasurebuf=5001;
$waveletstream=5002;
$outputmeasurest=5003;
$outputmeasurebuf=5004;

$gen="make_alternate.pl 'make_sine.pl 10 0 100 100 0.1' 'make_zeros.pl 100 0.1'";
#$gen="gen_numbers.pl 0.1";


#$cmd = "$gen | text2measure 100000 source:stdio:stdin target:stdio:stdout connect:tcp:$inputmeasurest| measurebuffer 10000 source:stdio:stdin server:tcp:$inputmeasurebuf connect:tcp:6789";

#$cmd = "$gen | text2measure 100000 source:stdio:stdin target:stdio:stdout connect:tcp:$inputmeasurest| measurebuffer 10000 source:stdio:stdin server:tcp:$inputmeasurebuf target:stdio:stdout | bin/I386/LINUX/wavelet_streaming_server daub8  5 transform source:stdio:stdin connect:tcp:$waveletstream";

$cmd = "$gen | text2measure 100000 source:stdio:stdin target:stdio:stdout connect:tcp:$inputmeasurest| measurebuffer 10000 source:stdio:stdin server:tcp:$inputmeasurebuf target:stdio:stdout | bin/I386/LINUX/wavelet_streaming_server daub8  4 transform source:stdio:stdin target:stdio:stdout connect:tcp:$waveletstream | bin/I386/LINUX/wavelet_streaming_client reconst source:stdio:stdin | text2measure 100000 source:stdio:stdin connect:tcp:$outputmeasurest target:stdio:stdout | measurebuffer 10000 source:stdio:stdin server:tcp:$outputmeasurebuf";


print "$cmd\n";

system $cmd;

