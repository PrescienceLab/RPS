#!/usr/bin/env perl
#
#
#
# expects to be run from the appropriate bin/ARCH/OS directory
# 
# tcppipeline_measure.pl loadserver_host loadserver_period_usec loadserver_ctrlport loadserver_outport load2measure_host load2measure_outport measurebuffer_host measurebuffer_depth measurebuffer_rrport measurebuffer_outport predserver_host predserver_rrport predserver_outport predbuffer_host predbuffer_depth predbuffer_rrport predbuffer_outport evalfit_host evalfit_stuff+
#
#
#

$RSH="ssh";

$DIR =`printenv PPSDIR`;
chomp($DIR);
$DIR.="/LoadMon/afsbin";

$sleeptime=15;

if ($#ARGV<18) {
    print STDERR "tcppipeline_measure.pl loadserver_host loadserver_period_usec loadserver_ctrlport loadserver_outport load2measure_host load2measure_outport measurebuffer_host measurebuffer_depth measurebuffer_rrport measurebuffer_outport predserver_host predserver_rrport predserver_outport predbuffer_host predbuffer_depth predbuffer_rrport predbuffer_outport evalfit_host evalfit_stuff+\n";
    exit;
}


$HOST=`hostname`;
chomp($HOST);

$loadserver_host=$ARGV[0];
$loadserver_period_usec=$ARGV[1];
$loadserver_ctrlport=$ARGV[2];
$loadserver_outport=$ARGV[3];
$load2measure_host=$ARGV[4];
$load2measure_outport=$ARGV[5];
$measurebuffer_host=$ARGV[6];
$measurebuffer_depth=$ARGV[7];
$measurebuffer_rrport=$ARGV[8];
$measurebuffer_outport=$ARGV[9];
$predserver_host=$ARGV[10];
$predserver_rrport=$ARGV[11];
$predserver_outport=$ARGV[12];
$predbuffer_host=$ARGV[13];
$predbuffer_depth=$ARGV[14];
$predbuffer_rrport=$ARGV[15];
$predbuffer_outport=$ARGV[16];
$evalfit_host=$ARGV[17];
@evalfit_stuff = @ARGV[18 .. $#ARGV];

$CMD = "$RSH $loadserver_host \"loadserver $loadserver_period_usec server:tcp:$loadserver_ctrlport connect:tcp:$loadserver_outport >& /tmp/loadserver.out\" &";

print STDERR "$CMD ; sleep($sleeptime)\n";

system $CMD;
sleep($sleeptime);

$CMD="$RSH $load2measure_host \"load2measure 0 source:tcp:$loadserver_host:$loadserver_outport connect:tcp:$load2measure_outport >& /tmp/load2measure.out  \"&";
print STDERR "$CMD ; sleep($sleeptime)\n";

system $CMD;
sleep($sleeptime);

$CMD = "$RSH $measurebuffer_host \"measurebuffer $measurebuffer_depth source:tcp:$load2measure_host:$load2measure_outport server:tcp:$measurebuffer_rrport connect:tcp:$measurebuffer_outport >& /tmp/measurebuffer.out \" &";
print STDERR "$CMD ; sleep($sleeptime)\n";

system $CMD;
sleep($sleeptime);

$CMD = "$RSH $predserver_host \"predserver source:tcp:$measurebuffer_host:$measurebuffer_outport source:tcp:$measurebuffer_host:$measurebuffer_rrport server:tcp:$predserver_rrport connect:tcp:$predserver_outport >& /tmp/predserver.out \" &";
print STDERR "$CMD ; sleep($sleeptime)\n";
system $CMD;
sleep($sleeptime);

$CMD = "$RSH $predbuffer_host \"predbuffer $predbuffer_depth source:tcp:$predserver_host:$predserver_outport server:tcp:$predbuffer_rrport connect:tcp:$predbuffer_outport >& /tmp/predbuffer.out\" &";
print STDERR "$CMD ; sleep($sleeptime)\n";
system $CMD;
sleep($sleeptime);

$CMD = "$RSH $evalfit_host \"evalfit source:tcp:$measurebuffer_host:$measurebuffer_outport  source:tcp:$predserver_host:$predserver_outport source:tcp:$predserver_host:$predserver_rrport";
foreach $thing (@evalfit_stuff) { 
    $CMD.=" $thing ";
}
$CMD.=" >& /tmp/evalfit.out\" &";
print STDERR "$CMD ; sleep($sleeptime)\n";
system $CMD;
sleep($sleeptime);

