#!/usr/local/bin/perl -w

$#ARGV==0 or die "usage: kill_experimentalsetup_on.pl host\n";

$host = $ARGV[0];

system "kill_hostloadmeasure_on.pl $host";
system "kill_genload_on.pl $host";
system "kill_spinserver_on.pl $host";
system "kill_hostloadpred_on.pl $host";
