#!/usr/local/bin/perl5 -w

$loadserver_ctrlport=5000;
$measurebuffer_rrport=5001;
$predserver_rrport=5002;
$loadserver_period_usec=1000000;
$predbuffer_rrport=5003;
$predbuffer_outport=5004;

$measurebuffer_depth=1000;
$predbuffer_depth=100;

$evalfit_stuff="30 50000 0.01 50 600 30 AR 16";

if ($#ARGV==0) { 
    $loadserver_period_usec=$ARGV[0];
}

system "unixdompipeline_measure.pl $loadserver_period_usec $loadserver_ctrlport $measurebuffer_depth $measurebuffer_rrport $predserver_rrport $predbuffer_depth $predbuffer_rrport $predbuffer_outport $evalfit_stuff > /dev/null 2>&1 ";

$host=`hostname`;
chomp($host);

$rate = 1/($loadserver_period_usec/1000000);

print "Started a loadpredictor with the following parameters:\n";
print " host:                            $host\n";
print " initial rate:                    $rate Hz\n";
print " evalfit parameters:              $evalfit_stuff\n\n";
print " measurement control at:          client:tcp:$host:$loadserver_ctrlport\n";
print " prediction control at:           client:tcp:$host:$predserver_rrport\n\n";
print " buffered measurement access at:  client:tcp:$host:$measurebuffer_rrport\n";
print " prediction stream access at:     source:tcp:$host:$predbuffer_outport\n";
print " buffered prediction access at:   client:tcp:$host:$predbuffer_rrport\n";

