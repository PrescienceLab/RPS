#!/usr/bin/env perl
use RPS;
use Getopt::Long;

$numtoprint=-1;

$usage="Get measurements on a list of sites using get_measurements_on.pl\n\n".
"usage: get_measurements_list.pl list\n\n".
"list is formatted as lines of hostname options*\n".RPSBanner();

$#ARGV==0 or die $usage;

open(LIST,$ARGV[0]);
$numhosts=0;
while (<LIST>) {
    if (!/\s*\#/) { 
	chomp;
	@fields=split;
	if ($#fields>=0) { 
	    $host[$numhosts] = $fields[0];
	    $otheropts[$numhosts] = join(" ", @fields[1 .. $#fields]);
	    $numhosts++;
	}
    }
}
close(LIST);



for ($i=0;$i<$numhosts;$i++) {
    $CMD = "get_measurements_on.pl $otheropts[$i] $host[$i]";
    open(MEASURE,"$CMD |");
    @data=<MEASURE>;
    close(PRED);
    print "$host[$i]($otheropts[$i]):";
    foreach $data (@data) {
      @s= split(/\s+/,$data);
      print " $s[2]";
    }
    print "\n";
}
