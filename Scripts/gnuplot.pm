package gnuplot;
require Exporter;
require FileHandle;
@ISA = qw(Exporter);
@EXPORT = qw(GnuplotScatterPlotFile GnuplotScatterPlot);

sub GnuplotScatterPlotFile {
  my $i;
  my %args = @_;
  my $file = $args{"file"} or die "no file to plot!";
  my $is3d = $args{"zcols"};
  my $outtype = defined $args{"outtype"} ? $args{"outtype"} : "eps";
  $outtype eq "Xwait" or $outtype eq "eps" or die "unknown outputtype";
  my $outfile = defined $args{"outfile"} ? $args{"outfile"} : "gnuplot.eps";
  my $xlabel = defined $args{"xlabel"} ? $args{"xlabel"} : "";
  my $ylabel = defined $args{"ylabel"} ? $args{"ylabel"} : "";
  my $zlabel = defined $args{"zlabel"} ? $args{"zlabel"} : "";
  my $title = defined $args{"title"} ? $args{"title"} : "";
  my $xcols =  $args{"xcols"} or die "no xcolumns!";
  my $ycols =  $args{"ycols"} or die "no ycolumns!";
  my $zcols =  $args{"zcols"};
  $#$xcols==$#$ycols or die "x and y cols don't match!";
  my $errcols = $args{"errcols"};
  !$errcols or $#$errcols==$#$xcols or die "wrong number of error cols!";
  my $symbols = $args{"symbols"};
  !$symbols or $#$symbols==$#$xcols or die "wrong number of symbols!";
  my $labels = $args{"labels"};
  !$labels or $#$labels==$#$xcols or die "wrong number of labels!";
  my $fontsize = defined $args{"fontsize"} ? $args{"fontsize"} : 18;
  my $xrange = defined $args{"xrange"} ? $args{"xrange"} : "[]";
  my $yrange = defined $args{"yrange"} ? $args{"yrange"} : "[]";
  my $zrange = defined $args{"zrange"} ? $args{"zrange"} : "[]";
  open(GNUPLOT,"|gnuplot");
  GNUPLOT->autoflush(1);
  STDIN->autoflush(1);
  if ($outtype eq "eps") {
    print GNUPLOT "set terminal postscript eps color dashed $fontsize\n";
    print GNUPLOT "set output \"$outfile\"\n";
  }
  print GNUPLOT "set title \"$title\"\n";
  print GNUPLOT "set xlabel \"$xlabel\"\n";
  print GNUPLOT "set ylabel \"$ylabel\"\n";
  my $cmd;
  if ($is3d) { 
      print GNUPLOT "set zlabel \"$zlabel\"\n";
      $cmd="splot $xrange $yrange $zrange";
  } else {
      $cmd="plot $xrange $yrange ";
  }
  for ($i=0;$i<=$#$xcols;$i++) { 
    if ($i>0) {
      $cmd.=",";
    }
    $cmd.=" \"$file\" using $xcols->[$i]:$ycols->[$i]";
    if ($is3d) {
	$cmd.=":$zcols->[$i]";
    }
    if ($errcols) { 
	$cmd.=":$errcols->[$i] ";
    }
    if ($labels) { 
      $cmd.=" title \"$labels->[$i]\" ";
    }
    if ($errcols) { 
      $cmd.=" with errorbars ";
    } else {
      if ($symbols) { 
	$cmd.=" with $symbols->[$i] ";
      }
    }
  }
#  print "$cmd\n";
  print GNUPLOT "$cmd\n";
  if ($outtype eq "Xwait") { 
    <STDIN>;
  }
  print GNUPLOT "quit\n";
  close(GNUPLOT);
}

sub MakeSlice {
  my ($begin,$step,$end) = @_;
  my $i;
  my @out;
  for ($i=$begin;$i<=$end;$i+=$step) { 
    push @out, $i;
  }
  return \@out;
}

sub GnuplotScatterPlot {
  my %args = @_;
  my %callargs = %args;
  my $data = $args{"data"} or die "no data!";
  my $haserrs = $args{"errs"};
  my $skip = $haserrs ? 3 : 2;
  my ($i,$j);
  my $maxnumcols = -1;
  my $numseries = ($#$data+1)/$skip;

  for ($i=0;$i<=$#$data;$i+=$skip) { 
    if (($#{$data->[$i]} != $#{$data->[$i+1]}) || ($haserrs && $#{$data->[$i]}!=$#{$data->[$i+2]})) {
      die "dataset $i has different numbers of x,y,err pts!";
    }
    if ($#{$data->[$i]} > $maxnumcols) { 
      $maxnumcols=$#{$data->[$i]}+1;
    }
  }

  open(OUT,">_gnuplottemp.txt");
  for ($j=0;$j<$maxnumcols;$j++) {
    for ($i=0;$i<=$#$data;$i+=$skip) { 
      if ($j>$#{$data->[$i]}) {
	print OUT "?\t?".($haserrs?"\t?":"");
      } else {		  
	print OUT "$data->[$i][$j]\t$data->[$i+1][$j]".($haserrs?"\t$data->[$i+2][$j]":"");
      }
    }
    print OUT "\n";
  }
  close(OUT);

  $callargs{"file"} = "_gnuplottemp.txt";
  $callargs{"xcols"} = MakeSlice(1,$skip,$#$data+1);
  $num=$#{$$callargs{"xcols"}};
  $callargs{"ycols"} = MakeSlice(2,$skip,$#$data+1);
  if ($haserrs) {
    $callargs{"errcols"} = MakeSlice(3,$skip,$#$data+1);
  }
  GnuplotScatterPlotFile(%callargs);
}




