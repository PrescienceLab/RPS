sub PlotCompare {
    my $title = shift;
    my $xlabel = shift;
    my $ylabel = shift;
    my $infile = shift;
    my $outfile = shift;

    open(MATLAB,"| matlab");
    MATLAB->autoflush(1);
    print MATLAB "m=dlmread('$infile','\\t');\n";
    print MATLAB "figure;\n";
    print MATLAB "set(gca,'FontSize',18);\n";
    my $plotcmd = "plot(";
    my $legendcmd = "legend(";
    my $first=1;
    while ($#_>-1) {
	my $label = shift;
	my $xcolnum=shift;
	my $ycolnum=shift;
	if (!$first) { 
	    $plotcmd.= ",";
	    $legendcmd.=",";
	} else {
	    $first=0; 
	}
	$plotcmd.="m(:,$xcolnum)+$tnomstep/2,m(:,$ycolnum)";
	$legendcmd.="'$label'";
    }
    $plotcmd.=");";
    $legendcmd.=");";
    print MATLAB "$plotcmd\n$legendcmd;\n";
    print MATLAB "title('$title','FontSize',18);xlabel('$xlabel','FontSize',18);ylabel('$ylabel','FontSize',18);\n";
    print MATLAB "print -deps $outfile\n";
    close(MATLAB);
}
	
sub PlotProportionCompareWithCI {
    my $title = shift;
    my $xlabel = shift;
    my $ylabel = shift;
    my $infile = shift;
    my $outfile = shift;
    my $ispercent = shift;

    open(MATLAB,"| matlab");
    MATLAB->autoflush(1);
    print MATLAB "m=dlmread('$infile','\\t');\n";
    print MATLAB "figure;\n";
    print MATLAB "set(gca,'FontSize',18);\n";
    my $legendcmd = "legend(";
    my $outcol=1;
    while ($#_>-1) {
	my $label = shift;
	my $xcolnum=shift;
	my $ycolnum=shift;
	my $numcol=shift;
	if ($outcol>1) { 
	    $legendcmd.=",";
	}
	print MATLAB "x(:,$outcol) = m(:,$xcolnum);\n";
	print MATLAB "y(:,$outcol) = m(:,$ycolnum);\n";
	if ($ispercent) { 
	    print MATLAB "fracs = m(:,$ycolnum)./100;\n";
	} else {
	    print MATLAB "fracs = m(:,$ycolnum)\n";
	}
	print MATLAB "counts = m(:,$numcol);\n";
	print MATLAB "ci(:,$outcol) = 1.96*sqrt((fracs.*(1-fracs))./counts);\n";
	if ($ispercent) { 
	    print MATLAB "ci(:,$outcol)=ci(:,$outcol)*100;\n";
        }
	$legendcmd.="'$label'";
	$outcol++;
    }
    $legendcmd.=");";
    my $plotcmd.="errorbar(x,y,ci,'--');";
    print MATLAB "$plotcmd\n$legendcmd;\n";
    print MATLAB "title('$title','FontSize',18);xlabel('$xlabel','FontSize',18);ylabel('$ylabel','FontSize',18);\n";
    print MATLAB "print -depsc $outfile\n";
    close(MATLAB);
}

    


