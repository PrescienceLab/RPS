#!/usr/bin/env perl

use FileHandle;

$#ARGV == 2 or die "usage sched_plot_stat_allhosts.pl metric tnomrange slackrange";
$metric = $ARGV[0];
$tnomrange = $ARGV[1];
$slackrange = $ARGV[2];

@scenarios = `mysql -u root --silent --batch -e "select name from descriptions" schedresults`;

system "rm -f junk.txt";
foreach $scenario (@scenarios)  {
    chomp($scenario);
#    print STDERR "scenario: $scenario\n";
    system "sched_gen_stat.pl $metric schedresults $scenario $tnomrange $slackrange | grep -v '%' >> junk.txt";
}

open(GNUPLOT,"|gnuplot");
GNUPLOT->autoflush(1);
STDIN->autoflush(1);

print GNUPLOT "set title '$metric for $tnomrange $slackrange'\n";
print GNUPLOT "set xlabel 'Scenarios'\n";
print GNUPLOT "set ylabel '$metric'\n";
print GNUPLOT "set xtics rotate (";
for ($i=0;$i<=$#scenarios;$i++) { 
    print GNUPLOT "\"$scenarios[$i]\" $i";
    if ($i<$#scenarios) { 
	print GNUPLOT ",";
    }
}
print GNUPLOT ")\n";
print GNUPLOT "plot 'junk.txt' using 8 title 'random', 'junk.txt' using 9 title 'measure', 'junk.txt' using 10 title 'mean','junk.txt' using 11 title 'last', 'junk.txt' using 12 title 'ar16'\n";

$result = <STDIN>;

if ($result =~ /^e\s+(.*)/) { 
    $file = $1;
    print GNUPLOT "set terminal postscript eps color dashed 18\n";
    print GNUPLOT "set output '$file'\n";
    print GNUPLOT "plot 'junk.txt' using 8 title 'random', 'junk.txt' using 9 title 'measure', 'junk.txt' using 10 title 'mean','junk.txt' using 11 title 'last', 'junk.txt' using 12 title 'ar16'\n";

}

print GNUPLOT "quit\n";


    






