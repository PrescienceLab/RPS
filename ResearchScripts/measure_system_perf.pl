#!/usr/bin/env perl

$maxhz=1024;

use FileHandle;

autoflush STDOUT 1;

$OUTDIR="/usr5/pdinda";

# kill everything
print STDERR "Kill all processes\n";
system "kill_pipeline.pl";

# start up the vmstat monitor
print STDERR "Start vmstat monitor\n";
system "clean_vmstat.pl > $OUTDIR/vmstat.out &";

# capture quiescent behavior
sleep(120);

# start up a load monitor
print STDERR "Start loadserver monitor\n";
system "clean_loadserver.pl 1000000 10000 > $OUTDIR/loadmonitor.out &";

# capture combined behavior
sleep(120);

# start up an all tcp prediction pipeline running at 1 Hz
# the evalfit paramams are set very high to assure that reconfig
# won't happen
print STDERR "Start tcp-based prediction pipeline\n";
system "tcppipeline_measure.pl pyramid 1000000 5000 5001 pyramid 5002 pyramid 1000 5003 5004 pyramid 5005 5006 pyramid 100 5007 5008 pyramid 30 999999999 1000.0 999999999 600 30 AR 16 ";

sleep(60);

if (0) { 
print STDERR "Start predclient\n";
system "predclient source:tcp:pyramid:5008 > /dev/null &";

# capture steady state behavior
sleep(600);


# Now ramp up rate and observe results
print STDERR "Sweep Hz for rate\n";
for ($hz=1;$hz<=$maxhz;$hz*=2) { 
    $usec = int(1000000/$hz); 
    print STDERR "$hz\t$usec\n"; 
    system "loadreconfig source:tcp:pyramid:5000 $usec"; 
    sleep(120); 
}

#reset to 1hz and let quiesce
print STDERR "Reset to 1 Hz\n";
system "loadreconfig source:tcp:pyramid:5000 1000000"; 
sleep(120);

#kill off the predclient and quiescing
print STDERR "Killing off predclient\n";
system "kill_matching.pl predclient";
sleep(120);

}

if (1) { 
    sleep(300);

#now ramp and test latencies
print STDERR "Sweep Hz for latency\n";
system "echo \"AR(16) 300 30\" > $OUTDIR/latency.out";
for ($hz=1;$hz<=$maxhz;$hz*=2) { 
    $usec = int(1000000/$hz); 
    print STDERR "$hz\t$usec\n";
    system "echo \"$hz Hertz\" >> $OUTDIR/latency.out";
    system "loadreconfig source:tcp:pyramid:5000 $usec"; 
    sleep(120);
    system "predlatclient source:tcp:pyramid:5008 300 stats >> $OUTDIR/latency.out";
}

#reset to 1hz and let quiesce
print STDERR "Reset to 1 Hz\n";
system "loadreconfig source:tcp:pyramid:5000 1000000"; 
sleep(120);

}

#kill everything
print STDERR "Kill everything\n";
system "kill_pipeline.pl";
system "kill_matching.pl clean_vmstat";
system "kill_matching.pl clean_loadserver";
system "kill_matching.pl measureclient";
system "kill_matching.pl load2measure";
system "kill_matching.pl loadserver";
system "kill_matching.pl measureclient";



