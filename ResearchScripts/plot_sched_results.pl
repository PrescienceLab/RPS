#!/usr/bin/env perl

use FileHandle;
use Getopt::Long;
use gnuplot;
use dataaccess;
use sched_dataaccess;

$skip=0;

$plotoutdir = ".";

&GetOptions("plotoutdir=s"=>\$plotoutdir, "nocombine" => \$nocombine, "skip" => \$skip, "conf"=>\$conf, "interactive"=>\$interactive, "autoscale"=>\$autoscale ) and $#ARGV>3 or die "usage: plot_sched_results.pl [--skip] [--conf] [--interactive] [--autoscale] [--nocombine] database table tnommin:tnomstep:tmaxstep slackmin:slackstep:slackmax models+\n";

$database = $ARGV[0];
$table = $ARGV[1];

($tnommin,$tnomstep,$tnommax) = split(/:/,$ARGV[2]);
($slackmin,$slackstep,$slackmax) = split(/:/,$ARGV[3]);

$outfilestem = "${database}_${table}_tnom$ARGV[2]_tslack$ARGV[3]";
$outfile = $outfilestem.".table";


@models = @ARGV[4 .. $#ARGV];

for ($tnom = $tnommin; $tnom < $tnommax; $tnom+=$tnomstep) { 
    push @tnoms, $tnom;
}
for ($slack = $slackmin; $slack< $slackmax; $slack+=$slackstep) {
    push @slacks, $slack;
}

@summarynames = ( "fracdeadlinesmet",
		  "fracdeadlinesmetci",
		  "fracdeadlinesmetwhilepossible",
		  "fracdeadlinesmetwhilepossibleci",
		  "fracinrange",
		  "fracinrangeci",
		  "frachigh",
		  "frachighci",
		  "fraclow",
		  "fraclowci",
                  "avgnumhosts",
		  "avgnumhostsci" );

$header="%tnommid\ttnommin\ttnommax\tslackmid\tslackmin\tslackmax";
$curcol = 7;
foreach $model (@models) { 
    $header.="\t${model}_numsamples\t${model}_fracdeadlinesmet\t${model}_fracdeadlinesmetci\t${model}_fracdeadlinemetwhilepossible\t${model}_fracdeadlinemetwhilepossible\t${model}_fracinrange\t${model}_fracinrangeci\t${model}_frachigh\t${model}_frachighci\t${model}_fraclow\t${model}_fraclowci\t${model}_avgnumhosts\t${model}_avgnumhostsci";
    $column{$model}{"tnommid"}=1;
    $column{$model}{"tnommin"} = 2;
    $column{$model}{"tnommax"} = 3;
    $column{$model}{"slackmid"}=4;
    $column{$model}{"slackmin"} = 5;
    $column{$model}{"slackmax"} = 6;
    $column{$model}{"numsamples"}=$curcol++;
    $column{$model}{"fracdeadlinesmet"}=$curcol++;
    $column{$model}{"fracdeadlinesmetci"}=$curcol++;
    $column{$model}{"fracdeadlinesmetwhilepossible"}=$curcol++;
    $column{$model}{"fracdeadlinesmetwhilepossibleci"}=$curcol++;
    $column{$model}{"fracinrange"}=$curcol++;
    $column{$model}{"fracinrangeci"}=$curcol++;
    $column{$model}{"frachigh"}=$curcol++;
    $column{$model}{"frachighci"}=$curcol++;
    $column{$model}{"fraclow"}=$curcol++;
    $column{$model}{"fraclowci"}=$curcol++;
    $column{$model}{"avgnumhosts"}=$curcol++;
    $column{$model}{"avgnumhostsci"} = $curcol++;
}
$header.="\n";


if ($skip) {
    print "skipping database queries and assuming files already exist\n";
    goto draw;
}


@col= GetColumnWhere($database,$table,"numpossible>=0","max(numpossible)");
$maxpossible =$col[0];

open(OUT,">$outfile");
print OUT "$header";

foreach $slack (@slacks) { 
    foreach $tnom (@tnoms) { 
	$tnommax = $tnom+$tnomstep;
	$tnommid = ($tnom+$tnommax)/2;
	$slackmax = $slack+$slackstep;
	$slackmid = ($slack+$slackmax)/2;
	print OUT "$tnommid\t$tnom\t$tnommax\t$slackmid\t$slack\t$slackmax";
	
	foreach $model (@models) { 
	    $where = " predname='$model' and tnom>=$tnom and tnom<($tnom+$tnomstep) and slack>=$slack and slack<($slack+$slackstep) ";
	    $countall = GetCountWhere($database, $table, $where);
	    $data{"count"}{$model}{$slack}{$tnom} = $countall;

	    print OUT "\t$countall";
	    
	    $countdeadlinesmet = GetCountWhere($database, $table, $where." and deadlinemet='met' ");
	    $fracdeadlinesmet = $countall==0 ? 0 : $countdeadlinesmet/$countall;
	    $fracdeadlinesmetci = $countall==0 ? 0 : 1.96*sqrt($fracdeadlinesmet*(1-$fracdeadlinesmet)/$countall);
	    print OUT "\t$fracdeadlinesmet\t$fracdeadlinesmetci";

	    $data{"fracdeadlinesmet"}{$model}{$slack}{$tnom} = $fracdeadlinesmet;
	    $data{"fracdeadlinesmetci"}{$model}{$slack}{$tnom} = $fracdeadlinesmetci;

	    $countpossible = GetCountWhere($database,$table,$where." and numpossible>0");
	    $countpossiblemet = GetCountWhere($database,$table,$where." and numpossible>0 and deadlinemet='met'");
	    $fracdeadlinesmetwhilepossible = $countpossible==0 ? 0 : $countpossiblemet/$countpossible;
	    $fracdeadlinesmetwhilepossibleci = $countpossible==0 ? 0 : 1.96*sqrt($fracdeadlinesmetwhilepossible*(1-$fracdeadlinesmetwhilepossible)/$countall);
	    $data{"fracdeadlinesmetwhilepossible"}{$model}{$slack}{$tnom} = $fracdeadlinesmetwhilepossible;
	    $data{"fracdeadlinesmetwhilepossibleci"}{$model}{$slack}{$tnom} = $fracdeadlinesmetwhilepossibleci;

	    print OUT "\t$fracdeadlinesmetwhilepossible\t$fracdeadlinesmetwhilepossibleci";

	    $countinrange = GetCountWhere($database,$table,$where." and inrange='in' ");
	    $fracinrange = $countall==0 ? 0 : $countinrange/$countall;
	    $fracinrangeci = $countall==0 ? 0 : 1.96*sqrt($fracinrange*(1-$fracinrange)/$countall);

	    $data{"fracinrange"}{$model}{$slack}{$tnom} = $fracinrange;
	    $data{"fracinrangeci"}{$model}{$slack}{$tnom} = $fracinrangeci;

	    print OUT "\t$fracinrange\t$fracinrangeci";

	    $counthigh = GetCountWhere($database,$table,$where." and inrange='HIGH' ");
	    $frachigh = $countall==0 ? 0 : $counthigh/$countall;
	    $frachighci = $countall==0 ? 0 : 1.96*sqrt($frachigh*(1-$frachigh)/$countall);

	    $data{"frachigh"}{$model}{$slack}{$tnom} = $frachigh;
	    $data{"frachighci"}{$model}{$slack}{$tnom} = $frachighci;

	    print OUT "\t$frachigh\t$frachighci";

	    $countlow = GetCountWhere($database,$table,$where." and inrange='LOW' ");
	    $fraclow = $countall==0 ? 0 : $countlow/$countall;
	    $fraclowci = $countall==0 ? 0 : 1.96*sqrt($fraclow*(1-$fraclow)/$countall);

	    $data{"fraclow"}{$model}{$slack}{$tnom} = $fraclow;
	    $data{"fraclowci"}{$model}{$slack}{$tnom} = $fraclowci;

	    print OUT "\t$fraclow\t$fraclowci";

	    if ($model eq "random") {
		$avgnumhosts = $maxpossible;
		$avgnumhostsci = 0;
	    } elsif ($model eq "measure") { 
		$avgnumhosts=1;
		$avgnumhostsci=0;
	    } else {
		@col = GetColumnWhere($database,$table,$where,"avg(max(1,numpossible))");
		$avgnumhosts=$col[0];
		@col = GetColumnWhere($database,$table,$where,"std(max(1,numpossible))");
		$stdnumhosts=$col[0];
		$avgnumhostsci=$countall==0 ? 0 : 1.96*$stdnumhosts/sqrt($countall);
	    }

	    $data{"avgnumhosts"}{$model}{$slack}{$tnom} = $avgnumhosts;
	    $data{"avgnumhostsci"}{$model}{$slack}{$tnom} = $avgnumhostsci;

	    print OUT "\t$avgnumhosts\t$avgnumhostsci";
	    
	}
	print OUT "\n";
    }
}
close(OUT);

open(OUT,">${outfile}.hash");
foreach $metric (keys %data) {
    foreach $model (keys %{$data{$metric}}) { 
	foreach $slack (keys %{$data{$metric}{$model}}) { 
	    foreach $tnom (keys %{$data{$metric}{$model}{$slack}}) { 
		print OUT "$metric $model $slack $tnom $data{$metric}{$model}{$slack}{$tnom}\n";
	    }
	}
    }
}
close(OUT);


draw:

open(IN,"${outfile}.hash");
while (<IN>) { 
    ($metric,$model,$slack,$tnom,$value) = split;
    $data{$metric}{$model}{$slack}{$tnom} = $value;
}
close(IN);


#goto draw2;
$outfiles="";
# draw summaries in 3D
foreach $name (@summarynames) { 
    if ( !($name =~ /ci$/)) { 
	$outfiles.=" $plotoutdir/$outfile.$name.surf.eps ";
	@args = (file => "$outfile",
		 outfile => "$plotoutdir/$outfile.$name.surf.eps",
		 xname => "tnommid",
		 yname => "slackmid",
		 zname => "$name",
		 models => \@models,
		 columns => \%column,
		 xlabel => "Nominal Time (seconds)",
		 ylabel => "Slack",
		 zlabel => "$name",
		 title => "$name for $outfile",
		 xrange=> "[0:$tnommax]",
		 yrange=> "[0:$slackmax]",
		 outtype => ($interactive ? "Xwait" : "eps"));
	if ($name =~ /^frac/ and !$autoscale) { 
	    push @args, zrange => "[0:1]";
	}
	if ($conf)  {
	    push @args, conf => $conf, confname=>"${name}ci" ;
	}
	PlotComparisonGraph(@args);
    }
}   

$nocombine or $interactive or system "(epsnup.pl --outfile=$plotoutdir/$outfile.surfaces.ps $outfiles; rm $outfiles)";

draw2:

$outfiles="";
# draw summaries in 2D
# first is with slacks held constant
foreach $name (@summarynames) { 
    $slackminfield = $column{$models[0]}{"slackmin"} - 1;
    foreach $slack (@slacks) {
	$slacknext=$slack+$slackstep;
	if ( !($name =~ /ci$/)) { 
	    $outfiles.=" $plotoutdir/$outfile.$name.slack$slack.eps ";
	    @args = (file => "< perl -ne '\@f=split; print if \$f[$slackminfield]==$slack' $outfile",
		     outfile => "$plotoutdir/$outfile.$name.slack$slack.eps",
		     xname => "tnommid",
		     yname => "$name",
		     models => \@models,
		     columns => \%column,
		     xlabel => "Nominal Time (seconds)",
		     ylabel => "$name",
		     title => "$name for $outfile with slack=$slack to $slacknext",
		     xrange=> "[0:$tnommax]",
		     outtype => ($interactive ? "Xwait" : "eps"));
	    if ($name =~ /^frac/ and !$autoscale) { 
		push @args, yrange => "[0:1]";
	    }
	    if ($conf)  {
		push @args, conf => $conf, confname=>"${name}ci" ;
	    }
	    PlotComparisonGraph(@args);
	}
    }
}

$nocombine or $interactive or system "(epsnup.pl --outfile=$plotoutdir/$outfile.constslack.ps $outfiles; rm $outfiles)";

draw3:

$outfiles="";
# draw summaries in 2D
# this is with tnom held constant
foreach $name (@summarynames) { 
    $tnomminfield = $column{$models[0]}{"tnommin"} - 1;
    foreach $tnom (@tnoms) {
	$tnomnext=$tnom+$tnomstep;
	if ( !($name =~ /ci$/)) { 
	    $outfiles.=" $plotoutdir/$outfile.$name.tnom$tnom.eps ";
	    @args = (file => "< perl -ne '\@f=split; print if \$f[$tnomminfield]==$tnom' $outfile",
		     outfile => "$plotoutdir/$outfile.$name.tnom$tnom.eps",
		     xname => "slackmid",
		     yname => "$name",
		     models => \@models,
		     columns => \%column,
		     xlabel => "Slack",
		     ylabel => "$name",
		     title => "$name for $outfile with tnom=$tnom to $tnomnext",
		     xrange=> "[0:$slackmax]",
		     outtype => ($interactive ? "Xwait" : "eps"));
	    if ($name =~ /^frac/ and !$autoscale) { 
		push @args, yrange => "[0:1]";
	    }
	    if ($conf)  {
		push @args, conf => $conf, confname=>"${name}ci" ;
	    }
	    PlotComparisonGraph(@args);
	}
    }
}
	
$nocombine or $interactive or system "(epsnup.pl --outfile=$plotoutdir/$outfile.consttnom.ps $outfiles; rm $outfiles)";

exit;	



# now generate output files

$outfiles="";
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


#
# reference to model list
# reference to column hash
# conf
#

sub PlotComparisonGraph {
    my %args = @_;
    my $xname = $args{"xname"};
    my $yname = $args{"yname"};
    my $zname = $args{"zname"};
    my $confname = $args{"confname"};
    my $models = $args{"models"};
    my $columns = $args{"columns"};
    my $conf = $args{"conf"};
    my @labels=();
    my @xcols=();
    my @ycols=();
    my @zcols=();
    my @errcols=();
    my @symbols=();
    my $model;
    foreach $model (@{$models}) { 
	push @labels, $model;
	push @xcols, $columns->{$model}{$xname};
	push @ycols, $columns->{$model}{$yname};
	$zname and push @zcols, $columns->{$model}{$zname};
	$confname and $conf and push @errcols, $columns->{$model}{$confname};
	push @symbols, "linespoints";
    }

    $args{"xcols"} or $args{"xcols"} = \@xcols;
    $args{"ycols"} or $args{"ycols"} = \@ycols;
    if ($zname) { 
	if (not defined $args{"zcols"}) { 
	    $args{"zcols"} =\@zcols;
	}
    }
    $args{"errcols"} or !$confname or !$conf or $args{"errcols"} = \@errcols;
    $args{"labels"} or $args{"labels"} = \@labels;
    if (not defined $args{"zcols"}) { 
	$args{"symbols"} or $args{"symbols"} = \@symbols;
    }
    return GnuplotScatterPlotFile(%args);
}



    
