#!/usr/local/bin/perl5 -w
#
#
#
# start_genload.pl [--tau=%lf] [--feedback=%lf] tracefile 
#
#
#

use Getopt::Long;

$usage = "start_genload.pl [--tau=%lf] [--feedback=%lf] tracefile";

$tau = 5.0;
$feedback=0.0;

&GetOptions(( "tau=f" => \$tau, "feedback=f" => \$feedback)) 
  and 
$#ARGV==0 
  and 
$tracefile=$ARGV[0] 
  or 
die "usage: $usage\n";

$COMMAND = "genload $tau $feedback $tracefile  > /dev/null 2>/dev/null";

print STDERR "$COMMAND\n";

system "$COMMAND &";


