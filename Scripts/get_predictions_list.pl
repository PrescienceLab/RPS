#!/usr/bin/env perl
use RPS;
use Getopt::Long;


$usage="Get predictions fom a list of places using get_predictions_on.pl\n\n".
"usage: get_predictions_list.pl [--num=num] list\n".RPSBanner();


$numtoprint=-1;

&GetOptions("num=i"=>\$numtoprint) and $#ARGV==0 
or die $usage;

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
    $CMD = "get_predictions_on.pl $otheropts[$i] $host[$i]";
    open(PRED,"$CMD |");
    @data=<PRED>;
    close(PRED);
    @pv = parsepreds(@data);
    $num = ($#pv+2)/2;
    if ($numtoprint>0) {
	$num = $num > $numtoprint ? $numtoprint : $num;
    }
    print "$host[$i]($otheropts[$i]):";
    for ($j=0;$j<$num;$j++) {
	print "\t$pv[$j]";
    }
    print "\n";
}



    

sub parsepreds {
    my @lines = @_;
    my $i;
    my @preds;
    my @vars;

    if ($#lines<2) { 
	return [];
    }

    for ($i=2;$i<=$#lines;$i++) { 
	chomp($lines[$i]);
	$lines[$i] =~ s/^\s+//g;
	($ahead,$pred,$var) = split(/\s+/,$lines[$i]);
	push @preds, $pred;
	push @vars,$var;
    }
    
    push @preds,@vars;
    
    return @preds;
}
