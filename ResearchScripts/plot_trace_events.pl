#!/usr/local/bin/perl5 -w

#
# plot_trace.pl tracefile event
#

use FileHandle;

$trace = $ARGV[0];
$eventpat = $ARGV[1];
open(TRACE,$trace);
open(PLOT,">$trace.plotdata");

$swall=0;
$s2wall=0;
$ssys=0;
$s2sys=0;
$susr=0;
$s2usr=0;
$stotal=0;
$s2total=0;

$n=0;
$firsttime=0;
while (<TRACE>) { 
    if (/^EE\s+\"$eventpat\"\s+\[(.+)\s+(.+)\s+(.+)\s+(.+)\]/) { 
	if ($n==0) { 
	    $firsttime=$1;
	}
	$time=$1 - $firsttime;
	$wall=$2;
	$sys=$3;
	$usr=$4;
	$total=$sys+$usr;
	print PLOT "$time $wall $sys $usr $total\n";
	$n++;
	$swall+=$wall;
	$s2wall+=$wall*$wall;
	$ssys+=$sys;
	$s2sys+=$sys*$sys;
	$susr+=$usr;
	$s2usr+=$usr*$usr;
	$stotal+=$total;
	$s2total+=$total*$total
    }
}
close(TRACE);
close(PLOT);

$meanwall=$swall/$n;
$meansys=$ssys/$n;
$meanusr=$susr/$n;
$meantotal=$stotal/$n;

$sdevwall=sqrt(($s2wall - $n*$meanwall*$meanwall)/($n-1));
$sdevsys=sqrt(($s2sys - $n*$meansys*$meansys)/($n-1));
$sdevusr=sqrt(($s2usr - $n*$meanusr*$meanusr)/($n-1));
$sdevtotal=sqrt(($s2total - $n*$meantotal*$meantotal)/($n-1));

print "walltime  = $meanwall ($sdevwall)\n";
print "systime   = $meansys ($sdevsys)\n";
print "usrtime   = $meanusr ($sdevusr)\n";
print "totaltime = $meantotal ($sdevtotal)\n";



if (1) {
open(GNUPLOT,"|gnuplot");
GNUPLOT->autoflush(1);

print GNUPLOT "set title \"$trace - $eventpat\"\n";
print GNUPLOT "set xlabel \"Timestamp (seconds)\"\n";
print GNUPLOT "set ylabel \"Seconds\"\n";

print GNUPLOT "plot \"$trace.plotdata\" using 1:2 title \"Walltime ($meanwall, $sdevwall)\" with lines, " 
                  ."\"$trace.plotdata\" using 1:3 title \"Systime ($meansys, $sdevsys)\" with lines, "
                  ."\"$trace.plotdata\" using 1:4 title \"Usrtime ($meanusr, $sdevusr)\" with lines, "
		      ."\"$trace.plotdata\" using 1:5 title \"Totaltime ($meantotal, $sdevtotal)\" with lines\n ";


$finish=<STDIN>;
if ($finish =~/^p\s+(.*)$/) {
    $file=$1;
    print GNUPLOT "set terminal postscript eps color \n";
    print GNUPLOT "set output \"$file\"\n";
    print GNUPLOT "set title \"$trace - $eventpat\"\n";
    print GNUPLOT "set xlabel \"Timestamp (seconds)\"\n";
    print GNUPLOT "set ylabel \"Seconds\"\n";
print GNUPLOT "plot \"$trace.plotdata\" using 1:2 title \"Walltime ($meanwall, $sdevwall)\" with lines, " 
                  ."\"$trace.plotdata\" using 1:3 title \"Systime ($meansys, $sdevsys)\" with lines, "
                  ."\"$trace.plotdata\" using 1:4 title \"Usrtime ($meanusr, $sdevusr)\" with lines, "
		      ."\"$trace.plotdata\" using 1:5 title \"Totaltime ($meantotal, $sdevtotal)\" with lines\n ";

} 

}
