#!/usr/local/bin/perl5 -w
#
#
#
# expects to be run from the appropriate bin/ARCH/OS directory
# 
# unixdompipeline_measure.pl loadserver_period_usec loadserver_ctrlport measurebuffer_depth measurebuffer_rrport predserver_rrport predbuffer_depth predbuffer_rrport predbuffer_outport evalfit_stuff+
#
#
#

#$RSH="ssh";

$DIR =`printenv PPSDIR`;
chomp($DIR);
$DIR.="/LoadMon/afsbin";

$sleeptime=5;

if ($#ARGV<7) {
    print STDERR "unixdompipeline_measure.pl loadserver_period_usec loadserver_ctrlport measurebuffer_depth measurebuffer_rrport predserver_rrport predbuffer_depth predbuffer_rrport predbuffer_outport evalfit_stuff+";
    exit;
}


$HOST="localhost";
chomp($HOST);

$loadserver_period_usec=$ARGV[0];
$loadserver_ctrlport=$ARGV[1];
$measurebuffer_depth=$ARGV[2];
$measurebuffer_rrport=$ARGV[3];
$predserver_rrport=$ARGV[4];
$predbuffer_depth=$ARGV[5];
$predbuffer_rrport=$ARGV[6];
$predbuffer_outport=$ARGV[7];
@evalfit_stuff = @ARGV[8 .. $#ARGV];

$CMD = "( ( rm -f /tmp/*.rps ; loadserver $loadserver_period_usec server:tcp:$loadserver_ctrlport target:stdio:stdout | load2measure 0 source:stdio:stdin target:stdio:stdout | measurebuffer $measurebuffer_depth source:stdio:stdin server:unix:/tmp/mbctrl.rps target:stdio:stdout server:tcp:$measurebuffer_rrport connect:unix:/tmp/measure.rps | predserver source:stdio:stdin source:unix:/tmp/mbctrl.rps server:unix:/tmp/psctrl.rps server:tcp:$predserver_rrport target:stdio:stdout connect:unix:/tmp/pred.rps | predbuffer $predbuffer_depth source:stdio:stdin server:tcp:$predbuffer_rrport connect:tcp:$predbuffer_outport) >/tmp/rps.main.out 2>&1  & ) ; sleep $sleeptime ;  ( ( evalfit source:unix:/tmp/measure.rps source:unix:/tmp/pred.rps client:unix:/tmp/psctrl.rps ";
foreach $thing (@evalfit_stuff) { 
    $CMD.=" $thing ";
}
$CMD.=" ) > /tmp/rps.eval.out 2>&1 & )";

print STDERR "$CMD\n";
system $CMD;

