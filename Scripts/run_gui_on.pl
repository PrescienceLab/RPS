#!/usr/bin/env perl

use Getopt::Long;

$usage = "run_gui_on.pl [--loadbufferport=port] [--predbufferport=port] host";

$loadbufferport = $ENV{"HOSTLOADSERVERBUFFERPORT"};
$predbufferport = $ENV{"HOSTLOADPREDBUFFERPORT"};


&GetOptions("loadbufferport=i"=>\$loadbufferport, 
            "predbufferport=i"=>\$predbufferport) ;

$#ARGV==0 and $host=$ARGV[0] or die "usage: $usage\n";

$CMD = "(export CLASSPATH=.; export LD_LIBRARY_PATH=.; cd \$RPS_DIR/JavaGUI; ./show_pred.sh client:tcp:$host:$loadbufferport client:tcp:$host:$predbufferport) & ";

#print "$CMD\n";

system $CMD;
