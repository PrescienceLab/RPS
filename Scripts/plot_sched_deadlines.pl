#!/usr/local/bin/perl -w

use FileHandle;
use Getopt::Long;

$skip=0;

&GetOptions("skip" => \$skip) and $#ARGV>3 or die "usage: plot_sched_deadlines.pl [--skip] database table tnommin:tnomstep:tmaxstep slackmin:slackstep:slackmax models+\n";

$database = $ARGV[0];
$table = $ARGV[1];

($tnommin,$tnomstep,$tnommax) = split(/:/,$ARGV[2]);
($slackmin,$slackstep,$slackmax) = split(/:/,$ARGV[3]);

$outfilestem = "${database}_${table}_tnom$ARGV[2]_tslack$ARGV[3]";

@models = @ARGV[4 .. $#ARGV];

for ($tnom = $tnommin; $tnom < $tnommax; $tnom+=$tnomstep) { 
    push @tnoms, $tnom;
}
for ($slack = $slackmin; $slack< $slackmax; $slack+=$slackstep) {
    push @slacks, $slack;
}


if ($skip) {
    print "skipping database queries and assuming files already exist\n";
    goto draw;
}

# first get total counts

foreach $model (@models) { 
    foreach $tnom (@tnoms) { 
	foreach $slack (@slacks) { 
	    $where = " predname='$model' and tnom>=$tnom and tnom<($tnom+$tnomstep) and slack>=$slack and slack<($slack+$slackstep) ";
	    $countall{$tnom}{$slack}{$model} = GetCountWhere($database, $table, $where);
#	    print "$model\t$tnom\t$slack\t$countall{$tnom}{$slack}{$model}\n";
	}
    }
}


# now get deadline meets
foreach $model (@models) { 
    foreach $tnom (@tnoms) { 
	foreach $slack (@slacks) { 
	    $where = " predname='$model' and tnom>=$tnom and tnom<($tnom+$tnomstep) and slack>=$slack and slack<($slack+$slackstep) and deadlinemet='met'";
	    $countdeadlinemet{$tnom}{$slack}{$model} = GetCountWhere($database, $table, $where);
#	    print "$model\t$tnom\t$slack\t$countall{$tnom}{$slack}{$model}\n";
	}
    }
}

# now generate output files

# now flatten to deadline meets vs tnom, slack 2d graphs
open(OUT,">${outfilestem}.deadlines_vs_tnom.out");
print OUT "% tnom";
foreach $model (@models) { 
    print OUT "\t$model";
}
print OUT "\n";
foreach $tnom (@tnoms) { 
    print OUT "$tnom";
    foreach $model (@models) { 
	$countall = 0;
	$countmet = 0;
	foreach $slack (@slacks) { 
	    $countall += $countall{$tnom}{$slack}{$model};
	    $countmet += $countdeadlinemet{$tnom}{$slack}{$model};
	}
	$percent = 100.0*($countall==0 ? 0 : $countmet/$countall);
	print OUT "\t$percent";
    }
    print OUT "\n";
}
close(OUT);

#now flatten to deadline meets vs slack	    
open(OUT,">${outfilestem}.deadlines_vs_slack.out");
print OUT "% slack";
foreach $model (@models) { 
    print OUT "\t$model";
}
print OUT "\n";
foreach $slack (@slacks) { 
    print OUT "$slack";
    foreach $model (@models) { 
	$countall = 0;
	$countmet = 0;
	foreach $tnom (@tnoms) { 
	    $countall += $countall{$tnom}{$slack}{$model};
	    $countmet += $countdeadlinemet{$tnom}{$slack}{$model};
	}
	$percent = 100.0*($countall==0 ? 0 : $countmet/$countall);
	print OUT "\t$percent";
    }
    print OUT "\n";
}
close(OUT);


# this generates an output table containing all the info
open(OUT,">${outfilestem}.table.out");
print OUT "% tnom\tslack";
foreach $model (@models) { 
    print OUT "\t$model";
}
print OUT "\n";
foreach $tnom (@tnoms) { 
    foreach $slack (@slacks) { 
	print OUT "$tnom\t$slack";
	foreach $model (@models) { 
	    $percentage= 100*($countall{$tnom}{$slack}{$model} == 0 ? 0.0 : $countdeadlinemet{$tnom}{$slack}{$model} / $countall{$tnom}{$slack}{$model});
	    print OUT "\t$percentage";
	}
	print OUT "\n";
    }
}
close(OUT);

#now generate surfaces for each model
foreach $model (@models) {
    open(OUT,">${outfilestem}.$model.surface.out");
    foreach $tnom (@tnoms) { 
	foreach $slack (@slacks) { 
	    $percentage= 100*($countall{$tnom}{$slack}{$model} == 0 ? 0.0 : $countdeadlinemet{$tnom}{$slack}{$model} / $countall{$tnom}{$slack}{$model});
	    print OUT "$percentage\n";
	}
	print OUT "\n";
    }
    close(OUT);
}


draw:

# OK, now let's get matlab to do some drawing
#open(MATLAB,"| matlab");

open(GNUPLOT, "| gnuplot");
GNUPLOT->autoflush(1);
STDIN->autoflush(1);
$cmd = "plot ";
$col=2;
foreach $model (@models) { 
    $cmd.= $col>2 ? "," : "";
    $cmd.=" \"$outfilestem.deadlines_vs_tnom.out\" using 1:$col title \"$model\" with linespoints";
    $col++;
}
print GNUPLOT "$cmd\n";
$junk=<STDIN>;
close(GNUPLOT);



sub GetCountWhere {
    my ($database, $table, $where) = @_;

    my $sql = "select count(*) from $table where $where";
    
    open(MYSQL, "mysql --user root --batch --silent --execute=\"$sql\" $database | ");
    my $line = <MYSQL>;
    close(MYSQL);
    chomp ($line);
    return $line;
}
