#!/usr/bin/env perl
use gnuplot;


#GnuplotScatterPlotFromFile(file => "thisisatest.txt");


GnuplotScatterPlotFile(file => "predresults_axp0_tnom0:2.5:10_.table",
			   outfile => "junk.eps",
			   xlabel => "tnom",
			   ylabel => "percentinrange",
			   title => "title",
			   xcols => [1,1],
                           ycols => [4,46],
                           labels => ["mean","ar16"],
		           outtype => "Xwait"
) ;


GnuplotScatterPlot(outfile => "junk2.eps",
		   xlabel => "X",
		   ylabel => "Y",
		   title => "mytitle",
		   data => [ [1, 2, 3, 4], [2, 4, 8, 16] ],
		   outtype => "Xwait" ) ;
