#!/usr/local/bin/perl -w

use FileHandle;
use Getopt::Long;
use gnuplot;
use simple_stats;
use dataaccess;
use predci_dataaccess;

$skip=0;
$plotoutdir = ".";
&GetOptions("plotoutdir=s"=>\$plotoutdir, "nocombine"=>\$nocombine, "skip" => \$skip, "conf"=>\$conf, "interactive"=>\$interactive) and $#ARGV>2 or die "usage: plot_predci_results.pl [--skip] [--conf] [--interactive] [--nocombine] database table tnommin:tnomstep:tmaxstep models+\n";

$database = $ARGV[0];
$table = $ARGV[1];

($tnommin,$tnomstep,$tnommax) = split(/:/,$ARGV[2]);

$outfilestem = "${database}_${table}_tnom$ARGV[2]_";
$outfile = $outfilestem.".table";


@models = @ARGV[3 .. $#ARGV];

for ($tnom = $tnommin; $tnom < $tnommax; $tnom+=$tnomstep) { 
    push @tnoms, $tnom;
}



# compute stats for each time slice
# maps from stat to the column we'll compute


# this gives us the order we want
@summarystatnames = ("avgrelerr",
		     "avgrelerrci",
#		     "stdrelerr",
		     "avgreplayerr",
		     "avgreplayerrci",
#		     "stdreplayerr",
		     "avgciwidth",
		     "avgciwidthci",
#		     "stdciwidth",
		     "avgciwidthvtexp",
		     "avgciwidthvtexpci",
#		     "stdciwidthvtexp",
		     "avgciwidthvtnom",
		     "avgciwidthvtnomci",
#		     "stdciwidthvtnom",
		     "avgciwidthvtact",
		     "avgciwidthvtactci"
#		     ,"stdciwidthvtact"
);

# this tells us how to tell sql to compute them

%summarystats = (avgrelerr=>"avg(abs(texp-tact)/texp)",
		 avgrelerrci=>"1.96*sqrt(std(abs(texp-tact)/texp)/count(*))",
		 stdrelerr=>"std(abs(texp-tact)/texp)",
		 avgreplayerr=>"avg(abs(tnom-usrtime)/tnom)",
		 avgreplayerrci=>"1.96*sqrt(std(abs(tnom-usrtime)/tnom)/count(*))",
		 stdreplayerr=>"std(abs(tnom-usrtime)/tnom)",
		 avgciwidth=>"avg((tub-tlb))",
		 avgciwidthci=>"1.96*sqrt(std(tub-tlb)/count(*))",
		 stdciwidth=>"std((tub-tlb))",
		 avgciwidthvtexp=>"avg((tub-tlb)/texp)",
		 avgciwidthvtexpci=>"1.96*sqrt(std((tub-tlb)/texp)/count(*))",
		 stdciwidthvtexp=>"std((tub-tlb)/texp)",
		 avgciwidthvtnom=>"avg((tub-tlb)/tnom)",
		 avgciwidthvtnomci=>"1.96*sqrt(std((tub-tlb)/tnom)/count(*))",
		 stdciwidthvtnom=>"std((tub-tlb)/tnom)",
		 avgciwidthvtact=>"avg((tub-tlb)/tact)",
		 avgciwidthvtactci=>"1.96*sqrt(std((tub-tlb)/tact)/count(*))",
		 stdciwidthvtact=>"std((tub-tlb)/tact)"
);


# these are the linear fits we shall do

@linfitnames = ("replayerrorvtusr",
		"relerrvtnom",
		"tactvtnom",
		"tactvtlb",
		"tactvtexp",
		"tactvtub",
		"ciwidthvtexp",
		"ciwidthvtnom",
		"ciwidthvtact");


# and these are the x and y columns they represent

%linfits = (replayerrorvtusr => ["tnom","(tnom-usrtime)/tnom"],
	    "relerrvtnom" => ["tnom", "(texp-tact)/texp"],
	    "tactvtnom" => ["tnom","tact"],
	    "tactvtlb" => ["tlb" , "tact"],
	    "tactvtexp" => ["texp" , "tact"],
	    "tactvtub" => ["tub" , "tact"],
	    "ciwidthvtexp" => ["texp","tub-tlb"],
	    "ciwidthvtnom" => ["tnom","tub-tlb"],
	    "ciwidthvtact" => ["tact","tub-tlb"] );




$header="%tnommid\ttnommin\ttnommax";
$curcol = 4;
foreach $model (@models) { 
    $header.="\t${model}_numsamples\t${model}_fracinci\t${model}_fracincici";
    $column{$model}{"tnommid"}=1;
    $column{$model}{"tnommin"} = 2;
    $column{$model}{"tnommax"} = 3;
    $column{$model}{"numsamples"}=$curcol++;
    $column{$model}{"fracinci"} = $curcol++;
    $column{$model}{"fracincici"} = $curcol++;
    foreach $name (@summarystatnames) {
	$header.="\t${model}_${name}";
	$column{$model}{$name} = $curcol++;
    }
    foreach $name (@linfitnames) {
	$header.="\t${model}_${name}_r2\t${model}_${name}_m\t${model}_${name}_b";
	$column{$model}{$name."_r2"} = $curcol++;
	$column{$model}{$name."_m"} = $curcol++;
	$column{$model}{$name."_b"} = $curcol++;

    }
}
$header.="\n";

if ($skip) {
    print "skipping database queries and assuming files already exist\n";
    goto draw;
}


open(TABLE,">$outfile");

print TABLE $header;

foreach $tnom (@tnoms) { 
    $tnomnext = $tnom+$tnomstep;
    $tnommid = ($tnom+$tnomnext)/2;
    $line="$tnommid\t$tnom\t$tnomnext";
    foreach $model (@models) { 
	$where = " model='$model' and tnom>=$tnom and tnom<($tnom+$tnomstep) ";
	$countall = GetCountWhere($database, $table, $where);
	$countmet = GetCountWhere($database, $table, $where." and tact>=tlb and tact<=tub");
	$fracinci = $countmet/$countall;
	$fracincici = 1.96 * sqrt($fracinci*(1-$fracinci)/$countall);
	$line.="\t$countall\t$fracinci\t$fracincici";
	foreach $name (@summarystatnames) { 
	    @col = GetColumnWhere($database,$table,$where,$summarystats{$name});
	    $line.="\t$col[0]";
	}
	foreach $name (@linfitnames) { 
	    @x = GetColumnWhere($database,$table,$where,${@{$linfits{$name}}}[0]);
  	    @y = GetColumnWhere($database,$table,$where,${@{$linfits{$name}}}[1]);
            ($r2,$m,$b) = LeastSquaresLine(\@x,\@y);
            $line.="\t$r2\t$m\t$b";
        }
    }
    $line.="\n";
    print TABLE $line;
}
close(TABLE);

draw:

$outfiles="";

$outfiles.=" $plotoutdir/$outfile.percent.ci.eps ";
PlotComparisonGraph(file => "$outfile",
		    outfile => "$plotoutdir/$outfile.percent.ci.eps",
		    xname => "tnommid",
		    yname => "fracinci",
		    confname => "fracincici",
		    models => \@models,
		    columns => \%column,
		    conf => $conf,
		    xlabel => "Nominal Time (seconds)",
		    ylabel => "Fraction in CI",
		    title => "Fraction in CI for $outfile",
		    yrange=> "[0:1]",
		    xrange=> "[0:$tnommax]",
		    outtype => ($interactive ? "Xwait" : "eps"));


foreach $name (@summarystatnames) {
    if (! ($name =~ /ci$/) ) {
    $outfiles.=" $plotoutdir/$outfile.$name.noci.eps ";
       PlotComparisonGraph(file => "$outfile",
	  		  outfile => "$plotoutdir/$outfile.$name.noci.eps",
                          xname => "tnommid",
                          yname => "$name",
 		          models => \@models,
		          columns => \%column,
		          xlabel => "Nominal Time (seconds)",
		          ylabel => "$name",
		          title => "$name for $outfile",
                          xrange=> "[0:$tnommax]",
		          outtype => ($interactive ? "Xwait" : "eps"));
      if ($name =~ /avg$/ and $conf) { 
      $outfiles.=" $plotoutdir/$outfile.$name.ci.eps ";
      PlotComparisonGraph(file => "$outfile",
                          outfile => "$plotoutdir/$outfile.$name.ci.eps",
                          xname => "tnommid",
                          yname => "$name",
                          confname => "${name}.ci",
 		          models => \@models,
		          columns => \%column,
			  conf => $conf,
		          xlabel => "Nominal Time (seconds)",
		          ylabel => "$name",
		          title => "$name (with cis) for $outfile",
                          xrange=> "[0:$tnommax]",
		          outtype => ($interactive ? "Xwait" : "eps"));
      }
}
}

$nocombine or $interactive or system "(epsnup.pl --outfile=$plotoutdir/$outfile.summary.ps $outfiles; rm $outfiles)";


$outfiles="";
foreach $linfit (@linfitnames) { 
    $r2col = $linfit."_r2";
    $mcol = $linfit."_m";
    $bcol = $linfit."_b";

    $outfiles.=" $plotoutdir/$outfile.${r2col}.eps ";
    PlotComparisonGraph(file => "$outfile",
                        outfile => "$plotoutdir/$outfile.${r2col}.eps",
                        xname => "tnommid",
                        yname => $r2col,
 		        models => \@models,
		        columns => \%column,
		        xlabel => "Nominal Time (seconds)",
		        ylabel => "average r2",
		        title => "$r2col for $outfile",
                        xrange=> "[0:$tnommax]",
		        outtype => ($interactive ? "Xwait" : "eps"));

if (0) {
    $outfiles.=" $plotoutdir/$outfile.${mcol}.eps ";
    PlotComparisonGraph(file => "$outfile",
                        outfile => "$plotoutdir/$outfile.${mcol}.eps",
                        xname => "tnommid",
                        yname => $mcol,
 		        models => \@models,
		        columns => \%column,
		        xlabel => "Nominal Time (seconds)",
		        ylabel => "average slope",
		        title => "$mcol for $outfile",
                        xrange=> "[0:$tnommax]",
		        outtype => ($interactive ? "Xwait" : "eps"));

    $outfiles.=" $plotoutdir/$outfile.${bcol}.eps ";
    PlotComparisonGraph(file => "$outfile",
                        outfile => "$plotoutdir/$outfile.${bcol}.eps",
                        xname => "tnommid",
                        yname => $bcol,
 		        models => \@models,
		        columns => \%column,
		        xlabel => "Nominal Time (seconds)",
		        ylabel => "average intercept",
		        title => "$bcol for $outfile",
                        xrange=> "[0:$tnommax]",
		        outtype => ($interactive ? "Xwait" : "eps"));
}
}

$nocombine or $interactive or system "(epsnup.pl --outfile=$plotoutdir/$outfile.linfits.ps $outfiles; rm $outfiles)";


#
# reference to model list
# reference to column hash
# conf
#

sub PlotComparisonGraph {
    my %args = @_;
    my $xname = $args{"xname"};
    my $yname = $args{"yname"};
    my $confname = $args{"confname"};
    my $models = $args{"models"};
    my $columns = $args{"columns"};
    my $conf = $args{"conf"};
    my @labels=();
    my @xcols=();
    my @ycols=();
    my @errcols=();
    my @symbols=();
    my $model;
    foreach $model (@{$models}) { 
	push @labels, $model;
	push @xcols, $columns->{$model}{$xname};
	push @ycols, $columns->{$model}{$yname};
	$confname and $conf and push @errcols, $columns->{$model}{$confname};
	push @symbols, "linespoints";
    }

    $args{"xcols"} or $args{"xcols"} = \@xcols;
    $args{"ycols"} or $args{"ycols"} = \@ycols;
    $args{"errcols"} or !$confname or !$conf or $args{"errcols"} = \@errcols;
    $args{"labels"} or $args{"labels"} = \@labels;
    $args{"symbols"} or $args{"symbols"} = \@symbols;

    return GnuplotScatterPlotFile(%args);
}



    
