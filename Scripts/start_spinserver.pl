#!/usr/bin/env perl
#
#
#
# start_spinserver.pl [--port=port] [--calibrationfile=calibrationfile]
#
#
#

$usage = "start_spinserver.pl [--port=port] [--calibrationfile=calibrationfile]";

use Getopt::Long;

$port=$ENV{"SPINSERVERPORT"};
$calfile="+/tmp/calibration.cal";

&GetOptions(("port=i" => \$port, "calibrationfile=s" => \$calfile))
  and
$#ARGV==-1
  or
die "usage: $usage\n";

$COMMAND = "spinserver $calfile server:tcp:$port > /dev/null 2>&1 ";

print STDERR "$COMMAND\n";

system "$COMMAND&";


