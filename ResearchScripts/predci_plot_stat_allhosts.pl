#!/usr/bin/env perl

use FileHandle;

$#ARGV == 1 or die "usage predci_plot_stat_allhosts.pl metric tnomrange";
$metric = $ARGV[0];
$tnomrange = $ARGV[1];

@hosts = `mysql -u root --silent --batch -e "select name from descriptions" predresults`;

system "rm -f junk.txt";
foreach $host (@hosts)  {
    chomp($host);
    system "pred_gen_stat.pl $metric predresults $host $tnomrange | grep -v '%' >> junk.txt";
}

open(GNUPLOT,"|gnuplot");
GNUPLOT->autoflush(1);
STDIN->autoflush(1);

print GNUPLOT "set title '$metric for $tnomrange'\n";
print GNUPLOT "set xlabel 'Hosts'\n";
print GNUPLOT "set ylabel '$metric'\n";
print GNUPLOT "set xtics rotate (";
for ($i=0;$i<=$#hosts;$i++) { 
    print GNUPLOT "\"$hosts[$i]\" $i";
    if ($i<$#hosts) { 
	print GNUPLOT ",";
    }
}
print GNUPLOT ")\n";
print GNUPLOT "plot 'junk.txt' using 6 title 'mean', 'junk.txt' using 7 title 'last', 'junk.txt' using 8 title 'ar16'\n";

$result = <STDIN>;

if ($result =~ /^e\s+(.*)/) { 
    $file = $1;
    print GNUPLOT "set terminal postscript eps color dashed 18\n";
    print GNUPLOT "set output '$file'\n";
    print GNUPLOT "plot 'junk.txt' using 6 title 'mean', 'junk.txt' using 7 title 'last', 'junk.txt' using 8 title 'ar16'\n";
}

print GNUPLOT "quit\n";


    






