#!/usr/bin/perl -w

$udpmeasureadx="239.239.239.239";
$udpmeasureport=5677;
$msport=5677;
$mbport=5678;  
$mblen=1000;
$predsport=5679;
$predbport=5680;
$predreconfigport=5681;
$predblen=1000;
# $predmodel="AR 16";  #default

$#ARGV==1 or die "usage: wt_pred.pl counter interval\n";

$counter=shift;
$interval=shift;  $intervalusec=1000000*$interval;

$cmd="WatchTowerRPS -x ".$interval."s \"$counter\" | text2measure $intervalusec source:stdio:stdin target:stdio:stdout connect:tcp:$msport target:udp:$udpmeasureadx:$udpmeasureport | measurebuffer $mblen source:stdio:stdin server:tcp:$mbport target:stdio:stdout | predserver source:stdio:stdin client:tcp:localhost:$mbport server:tcp:$predreconfigport target:stdio:stdout connect:tcp:$predsport| predbuffer $predblen source:stdio:stdin server:tcp:$predbport";

print $cmd, "\n";

system $cmd;



