#!/usr/bin/env perl

use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;

my $db = rps_env->InitENV("bufferclients");

$db->RPSDBinit($ENV{"RPSDB_USER"},$ENV{"RPSDB_PASSWD"},$ENV{"RPSDB_PATH"});

my $cgi = new CGI;


my $resource = $cgi->param("resource");
my $resourcetype = $cgi->param("resourcetype");

if ($resourcetype eq "") {$resourcetype = "host load";}

my @resources = $db->RPSDBaddresses($resourcetype);

#this will be the first img name for the graph
#this is a dirty dirty way of stopping the browser
#from caching the images. should probably
#find a less lame fix.
my $pid = $$;
my $timefilename = $pid;


printHeading();

my ($predbufferchecked,
    $measurebufferchecked,
    $noerror,
    $predbufferclientnum,
    $measurebufferclientnum,
    $numgraphs,
    $graphchecked,
    $textchecked) = printMainForm(@resources);

if ($graphchecked && $textchecked && $resource && $resourcetype)
 {
  graphAndPrintData($resource,
                    $resourcetype,
		    $numgraphs,
		    $measurebufferclientnum,
		    $predbufferclientnum,
		    $predbufferchecked,
		    $measurebufferchecked,
                    $noerror);
 }
elsif ($graphchecked && $resource && $resourcetype)
 {
  graphData($resource,
            $resourcetype, 
	    $numgraphs, 
            $measurebufferclientnum, 
            $predbufferclientnum, 
            $predbufferchecked, 
            $measurebufferchecked,
            $noerror);
 }
elsif ($textchecked && $resource && $resourcetype)
 {
  printData($resource, 
            $resourcetype,
            $numgraphs, 
            $measurebufferclientnum, 
            $predbufferclientnum, 
            $predbufferchecked, 
            $measurebufferchecked,
            $noerror);
 }

print $cgi->end_html();

###############################################
###############################################
sub graphAndPrintData
 {
  my ($resource,
      $resourcetype,
      $numgraphs,
      $measurebufferclientnum,
      $predbufferclientnum, 
      $predbufferchecked, 
      $measurebufferchecked,
      $noerror) = @_;

  my @systemcalls;
  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource, $resourcetype);
  if ($measurebufferchecked && $predbufferchecked)
   {
    graphAndPrintBoth($noerror,@systemcalls);
   }
  elsif($measurebufferchecked)
   {
    graphAndPrintMeas(@systemcalls);
   }
  elsif($predbufferchecked)
   {
    graphAndPrintPred($noerror,@systemcalls);
   }
 }


sub graphAndPrintBoth
 {
  my ($noerror,@systemcalls) = @_;
  my @graphs;
  my $measureData="";
  
  #zerotime and predzerotime are used to normalize the graphs x-axes
  #zerotime is based on the measurement buffer and is assumed to be
  #more reliable than the prediction buffer as an estimate of the
  #time of the current time.
  
  my  $zerotime = 0;
  my $predzerotime= 0;

  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      my @output = `$_ 2>&1`;
      ($measureData,$zerotime) = genGnuMeasureData(@output);
     }
    elsif($_ =~ m/predbufferclient/)
     {
       my @output = `$_ 2>&1`;
       ($predzerotime,@graphs) = genGnuPredData(@output);
     }
   }
  my $i = @graphs-1;
  $numgraphs = @graphs;

  my $rows = floor(max(18,29/$numgraphs));

  for(my $j=0; $j < @graphs ; $j++)
   {
    my ($preddata, $prederror) = separateDataAndError($graphs[$i],$zerotime);
    if ($noerror){$prederror = "";}
    print "$j steps behind:";
    print "<img src =\"" . genGnuPlot($timefilename . 
				      $j, 
				      $measureData , 
				      $preddata,
                                      $prederror, 
				      $numgraphs, 
				      $zerotime) 
	   . "\"</img>\n";
    print "<textarea rows = $rows cols = 30>";
    if ($noerror)
     {
    print "MeasureData:\n" . normalize($measureData,$zerotime) 
           . "Prediction Data: \n$preddata";
    print "</textarea></br>";
     }
    else
     {
    print "MeasureData:\n" . normalize($measureData,$zerotime) 
           . mergeDataAndError($graphs[$i],$zerotime);
    print "</textarea></br>";
     }
    $i--;
   }
 }

sub graphAndPrintMeas
 {
  my (@systemcalls) = @_;
  my $cols;
  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      $numgraphs = 1;
      my @output = `$_ 2>&1`;
      my ($measureData,$zerotime) = genGnuMeasureData(@output);
      print "Measurements: <br>";
      print "<img src =\"" . genGnuPlot($timefilename . "0", 
                                        $measureData,
					"", 
					"",
                                        1,
                                        $zerotime, 
					"",
                                        "Measurements") 
                           . "\"</img>\n";
    print "<textarea rows = 29 cols = 30>";
    print "Measurements:\n" . normalize($measureData,$zerotime);
    print "</textarea></br>";
     }
   }
 }

sub graphAndPrintPred
 {
  my ($noerror,@systemcalls) = @_;
  my @graphs;
  my $predzerotime;
  
  foreach(@systemcalls)
   {
    if($_ =~ m/predbufferclient/)
     {
      my @output = `$_ 2>&1`;
      ($predzerotime,@graphs) = genGnuPredData(@output);
     }
   }
  my $i = @graphs-1;
  $numgraphs = @graphs;

  my $rows = floor(max(18,29/$numgraphs));

  for(my $j=0; $j < @graphs ; $j++)
   {
    my ($preddata, $prederror) = separateDataAndError($graphs[$i],$predzerotime);
    if ($noerror){$prederror = "";}
    print "$j steps behind:";
    print "<img src =\"" . genGnuPlot($timefilename . $j, 
				      "",
                                      $preddata,
                                      $prederror, 
                                      $numgraphs, 
                                      $predzerotime,
                                      "Predictions",
                                      "") 
                         . "\"</img>\n";

    print "<textarea rows = $rows cols = 30>";
    if ($noerror)
     {
    print "Prediction Data: \n$preddata";
     }
    else
     {
    print mergeDataAndError($graphs[$i],$predzerotime);
     }
    print "</textarea></br>";
    $i--;
   }
 }


sub graphData
 {
  my ($resource, 
      $resourcetype,
      $numgraphs, 
      $measurebufferclientnum, 
      $predbufferclientnum, 
      $predbufferchecked, 
      $measurebufferchecked,
      $noerror) = @_;
  
  my @systemcalls;
  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource,$resourcetype);

  if ($measurebufferchecked && $predbufferchecked)
   {
    graphBoth($noerror,@systemcalls);
   }
  elsif($measurebufferchecked)
   {
    graphMeas(@systemcalls);
   }
  elsif($predbufferchecked)
   {
    graphPred($noerror,@systemcalls);
   }
 }

sub graphBoth
 {
  my ($noerror,@systemcalls) = @_;
  my @graphs;
  my $measureData="";

  #zerotime and predzerotime are used to normalize the graphs x-axes
  #zerotime is based on the measurement buffer and is assumed to be
  #more reliable than the prediction buffer as an estimate of the
  #time of the current time.
  
  my  $zerotime = 0;
  my $predzerotime= 0;

  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      my @output = `$_ 2>&1`;
      ($measureData,$zerotime) = genGnuMeasureData(@output);
     }
    elsif($_ =~ m/predbufferclient/)
     {
       my @output = `$_ 2>&1`;
       ($predzerotime,@graphs) = genGnuPredData(@output);
     }
   }
  my $i = @graphs-1;
  $numgraphs = @graphs;

  for(my $j=0; $j < @graphs ; $j++)
   {
    my ($preddata, $prederror) = separateDataAndError($graphs[$i],$zerotime);
    if($noerror){$prederror="";}
    print "$j steps behind:";
    print "<img src =\"" . genGnuPlot($timefilename . 
				      $j, 
				      $measureData , 
				      $preddata,
                                      $prederror, 
				      $numgraphs, 
				      $zerotime) 
	   . "\"</img><br>\n";
    $i--;
   }
 }

sub graphMeas
 {
  my (@systemcalls) = @_;

  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      $numgraphs = 1;
      my @output = `$_ 2>&1`;
      my ($measureData,$zerotime) = genGnuMeasureData(@output);
      print "Measurements: <br>";
      print "<img src =\"" . genGnuPlot($timefilename . "0", 
                                        $measureData,
					"",
					"", 
                                        1,
                                        $zerotime, 
                                        "",
					"Measurements") 
                           . "\"</img><br>\n";
     }
   }
 }

sub graphPred
 {
  my ($noerror,@systemcalls) = @_;
  my @graphs;
  my $predzerotime;
  
  foreach(@systemcalls)
   {
    if($_ =~ m/predbufferclient/)
     {
      my @output = `$_ 2>&1`;
      ($predzerotime,@graphs) = genGnuPredData(@output);
     }
   }
  my $i = @graphs-1;
  $numgraphs = @graphs;

  for(my $j=0; $j < @graphs ; $j++)
   {
    my ($preddata, $prederror) = separateDataAndError($graphs[$i],$predzerotime);
    if($noerror){$prederror="";}
    print "$j steps behind:";
    print "<img src =\"" . genGnuPlot($timefilename . $j, 
				      "",
                                      $preddata,
				      $prederror, 
                                      $numgraphs, 
                                      $predzerotime) 
                         . "\"</img><br>\n";
    $i--;
   }
 }


sub genGnuMeasureData
 {
  my (@output) = @_;
  my $gnuMeasureData = "";
  my $xtime = 0;
  my @templine;
    
  foreach (@output)
   {  
    if ($xtime > -$measurebufferclientnum)
     {   
      @templine = split(' ',$_);
      $gnuMeasureData.= timeFloor($templine[1]) . " " . $templine[2] . " ! ";
      $xtime--;
     }
   }

  #after we're done parsing the data, return the latest time value (this will be
  #"zero time"

  my $zerotime = timeFloor($templine[1]);

  #ok, so i was having trouble parsing this later on with just "\n" (split gets rid of \n's)
  #so, i replaced it with "!" and parsed that out in the normalize() subroutine.
 
  return ($gnuMeasureData . " ! ",$zerotime);
 }


sub genGnuPredData
 {
  my (@output) = @_;
  my $gnuPlotData = "";
  my $gnuPlotUpError = "";
  my $gnuPlotLowError = "";
  my $index = 1; 
  my $bufferindex=0;  
  my @graphs; 


  #get the base value of x (first prediction vector's time in seconds)
  my $xbase = parseTime($output[0]);
    
  #get the length of each prediction vector that's being fed to us
  my $N = parseN($output[0]);
    
  foreach (@output)
   {
    if($index > 2)
     {
      my @templine = split(' ',$_);
     
      if ($index - 2 <= $predbufferclientnum)
       {
        #if the error is less than 3x the data, plot it
        #otherwise just plot the data
             
        if ($templine[2] < $templine[1]*3)
	 {
          my $uperr = $templine[2] + $templine[1];
          my $derr = -$templine[2] + $templine[1];
          $gnuPlotUpError .= $index - 2 + $xbase . " " . $uperr . " ! ";
          $gnuPlotLowError .= $index - 2 + $xbase . " " . $derr . " ! ";
         }
           
        $gnuPlotData .= $index - 2 + $xbase. " " . $templine[1] . " ! ";
       }
      if ($index >= $N + 2)
       {
        $index = 1;
        push(@graphs,$gnuPlotData . " ! " . $gnuPlotUpError . " ! " . $gnuPlotLowError . " ! ");
        $gnuPlotData="";
        $gnuPlotUpError="";
        $gnuPlotLowError="";
        $xbase++;
       }
      else
       {
        $index++;
       }    
     }
    else
     { 
      $index++;
     }
   }
  my $predzerotime=$xbase;
  return ($predzerotime,@graphs);
 }


sub genGnuPlot
 {
  my($j,$measdata,$preddata,$prederror,$numgraphs, $zerotime) = @_;
  my $datafile = $j;
  my $gnuplotfile = $j;
  my $jpegfile = $j;
  my $x = 600/sqrt($numgraphs);
  my $y = 450/sqrt($numgraphs);
  my $plotscript = "";


  #the data comes in with the x-axis on the unix time scale
  #i.e. (12341243 firstvalue 12341244 secondvalue ...), we want
  #to normalize these values around $zerotime (the first value obtained
  #from the measurebufferclient)


  my $normalizemeasdata = normalize($measdata, $zerotime);
 
 
  #try to make sure that if more than one graph is requested we can fit two
  #graphs side by side in an average window.

  if ($numgraphs > 1 && $x < 300)
   {
    $x = 300;
    $y = 225;
   }
  #generate three plots out of the data (data, uppererror and lower error)
  if ($preddata ne "" && $normalizemeasdata ne "")
   {
    open(MYOUTFILE, ">gnuplot/P$datafile.dat");
    print MYOUTFILE $preddata;
    close(MYOUTFILE);
    open(MYOUTFILE, ">gnuplot/M$datafile.dat");
    print MYOUTFILE $normalizemeasdata;
    close(MYOUTFILE);
    open(MYOUTFILE, ">gnuplot/E$datafile.dat");
    print MYOUTFILE $prederror;
    close(MYOUTFILE);
    $plotscript = "plot \'gnuplot/P$datafile.dat\' using 1:2 title \'Predictions\' with lines," .
                       "\'gnuplot/E$datafile.dat\' using 1:2 title \'Prediction Error\' with lines," .
                       "\'gnuplot/M$datafile.dat\' using 1:2 title \'Measurements\' with lines\n";
   } 
  elsif ($preddata ne "")
   {
    open(MYOUTFILE, ">gnuplot/P$datafile.dat");
    print MYOUTFILE $preddata;
    close(MYOUTFILE);
    open(MYOUTFILE, ">gnuplot/E$datafile.dat");
    print MYOUTFILE $prederror;
    close(MYOUTFILE);
    $plotscript = "plot \'gnuplot/P$datafile.dat\' using 1:2 title \'Predictions\' with lines," .
                       "\'gnuplot/E$datafile.dat\' using 1:2 title \'Prediction Error\' with lines\n";
   }
  elsif ($normalizemeasdata ne "")
   {
    open(MYOUTFILE, ">gnuplot/M$datafile.dat");
    print MYOUTFILE $normalizemeasdata;
    close(MYOUTFILE);
    $plotscript = "plot \'gnuplot/M$datafile.dat\' using 1:2 title \'Measurements\' with lines\n";
   }

  open(MYOUTFILE, ">gnuplot/$gnuplotfile");
#
#
# Changed to use PNGs for portability -PAD
#
  print MYOUTFILE "set terminal png small transparent color " .
#  print MYOUTFILE "set terminal jpeg transparent small size $x,$y " .
  "xffffff x000000 xadd8e6 " .
  "x9500d3 \n" .
  "set output \"gnuplot/$jpegfile.png\" \n" .
  "set xlabel \'time\' \n" .
  "set ylabel \'values\' \n";
  print MYOUTFILE $plotscript;
  
  close(MYOUTFILE);

  system("gnuplot gnuplot/$gnuplotfile");

  return "gnuplot/" . $jpegfile . ".png";
 }

sub separateDataAndError
 {
  my ($data,$zerotime) = @_;
  my $preddata = "";
  my $prederror = "";
  my $normalizeData = normalize($data,$zerotime);
  my @data = split(' ', $normalizeData);
  my @reversedata = reverse(@data);
  my $errorIndex = -1;

  my $sentinel = $data[0];

  for(my $i = 1; $i < @data; $i++)
   {
    if ($data[$i] == $sentinel && $i % 2 == 0)
     {
      $errorIndex = $i;
     }
   }

  $errorIndex = -(@data) + $errorIndex*2;
  my $numberErrors = (@data - $errorIndex);
  if ($errorIndex > 0)
  {
   for (my $i = 0 ; $i < $errorIndex ; $i++)
    {
     if ($i % 2 == 0 || $i == 0)
      {
       $preddata .= $data[$i] . " ";
      }
     else
      {
       $preddata .= $data[$i] . "\n";
      }  
    }

   for (my $i = $errorIndex ; $i < @data ; $i++)
    {
     if($i == $errorIndex + $numberErrors/2)
      {
       $prederror .= "\n" . $data[$i] . " ";
      }
     elsif ($i % 2 == 0 || $i == $errorIndex)
      {
       $prederror .= $data[$i] . " ";
      }
     else
      {
       $prederror .= $data[$i] . "\n";
      }  
    }
   return($preddata,$prederror);
  }
  else
   {
    return ("0 0", "");
   }
 }


sub mergeDataAndError
 {
  my ($data, $zerotime) = @_;

  my $mergeData = "";
  my $normalizeData = normalize($data,$zerotime);
  my @data = split(' ', $normalizeData);
  my @reversedata = reverse(@data);
  my $errorIndex = -1;

  my $sentinel = $data[0];

  for(my $i = 1; $i < @data; $i++)
   {
    if ($data[$i] == $sentinel && $i % 2 == 0)
     {
      $errorIndex = $i;
     }
   }
  $errorIndex = -(@data) + $errorIndex*2;
  my $numberErrors = (@data - $errorIndex);

  for (my $i = 0 ; $i < @data ; $i++)
   {
    if ($i == 0)
     {
      $mergeData .= "Prediction Data: \n" . $data[$i] . " ";
     }
    elsif ($i == $errorIndex)
     {
      $mergeData .= "\nUpper Error: \n" . $data[$i] . " ";
     }
    elsif($i == $errorIndex + $numberErrors/2)
     {
      $mergeData .= "\nLower Error: \n" . $data[$i] . " ";
     }
    elsif($i % 2 == 0)
     {
      $mergeData .= $data[$i] . " ";
     }
    else
     {
      $mergeData .= $data[$i] . "\n";
     }  
    }
  return $mergeData;
 }

sub normalize
 {
  my ($data, $zerotime) = @_;
  my @data = split(' ',$data);
  my $normalizeData = "";
  my $index = 2;
  foreach (@data)
   {
    if($_ eq '!')
     {
      $normalizeData.="\n";
     }
    elsif($index%2==0)
     {
      $normalizeData.=$_ - $zerotime . " ";
      $index ++;
     }
    else
     {
      $normalizeData.="$_";
      $index ++;
     }
   }
  return $normalizeData;
 }

sub parseN
 {
  my ($unparsed) = @_;
  my @tempx = split('N\(',$unparsed);
  @tempx = split('\)',$tempx[1]);
  return $tempx[0];
 }

#...P(1243655364.345345)... => 1243655364

sub parseTime
 {
  my ($unparsed) = @_;
  my @tempx = split('P\(',$unparsed);
  @tempx = split(' ',$tempx[1]);
  return timeFloor($tempx[0]);
 } 

#123456.12345 => 123456

sub timeFloor
 {
  my ($time) = @_;
  my @seconds = split('\.',$time);
  return $seconds[0];
 }

sub floor
 {
   my ($value) = @_;
   my @split = split('\.',$value);
   return $split[0];
 }

sub max
 {
  my ($one, $two) = @_;
  if ($one > $two)
   {
    return $one;
   }
  else
   {
    return $two;
   }
 }
############################################################

sub printData
 {
  my ($resource, 
      $resourcetype,
      $numgraphs, 
      $measurebufferclientnum, 
      $predbufferclientnum, 
      $predbufferchecked, 
      $measurebufferchecked) = @_;
  my @systemcalls;

  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource, $resourcetype);
  if ($measurebufferchecked && $predbufferchecked)
   {
    printBoth(@systemcalls,$numgraphs,$measurebufferclientnum,$predbufferclientnum);
   }
  elsif($measurebufferchecked)
   {
    printMeas(@systemcalls,$numgraphs,$measurebufferclientnum,$predbufferclientnum);
   }
  elsif($predbufferchecked)
   {
    printPred(@systemcalls,$numgraphs,$measurebufferclientnum,$predbufferclientnum);
   }
 }

sub printBoth
 {
  my (@systemcalls, $numgraphs, $measurebufferclientnum, $predbufferclientnum) = @_;

  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      print "<b>Measurements:</b><br>";
     }
    if($_ =~ m/predbufferclient/)
     {
      print "<b>Predictions:</B><br>";
     }
    my @output = `$_ 2>&1`;
    foreach (@output)
     {
      print $_ . "<br>";
     }
   }
 }

sub printMeas
 {
  my (@systemcalls, $numgraphs, $measurebufferclientnum, $predbufferclientnum) = @_;

  foreach(@systemcalls)
   {
    if($_ =~ m/measurebufferclient/)
     {
      print "<b>Measurements:</b><br>";
      my @output = `$_ 2>&1`;
      foreach (@output)
       {
        print $_ . "<br>";
       }
     }
   }
 }

sub printPred
 {
  my (@systemcalls, $numgraphs, $measurebufferclientnum, $predbufferclientnum) = @_;

  foreach(@systemcalls)
   {
    if($_ =~ m/predbufferclient/)
     {
      print "<b>Predictions:</b><br>";
      my @output = `$_ 2>&1`;
      foreach (@output)
       {
        print $_ . "<br>";
       }
     }
   }
 }


#############################################################

sub printHeading
 {


  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head><META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">" .
              "<META NAME=\"REFRESH\" CONTENT=\"1\">".
              "<title>RPS Web Interface</title>
              <script type=text/javascript>
               function submitToCGI1()
                {
                 document.mainForm.action = \"rpswebinterface.pl\";
                 document.mainForm.submit();
                }
               function submitToCGI2()
                {
                 document.mainForm.action = \"delete.pl\";
                 document.mainForm.submit();
                }
               function submitToCGI3()
                {
                 location.replace(\"add.pl\");
                }
	       function submitToCGI4()
		{
		 document.mainForm.action= \"autorefresh.pl\";
		 document.mainForm.submit();
		}
	       function submitToCGI5()
		{
		 document.mainForm.predbufferclient.checked = false;
		 document.mainForm.measurebufferclient.checked = false;
		 document.mainForm.submit();
		}
               function submitToCGI6()
                {
                 document.mainForm.action=\"stream.pl\";
                 document.mainForm.submit();
                }
              </script>
         </head>";
 }

sub printMainForm
 {
  my (@resources) = @_;

  my $predbufferchecked =0;
  my $measurebufferchecked = 0;
  my $noerror = 0;
  my $predbufferclientnum = 100;
  my $measurebufferclientnum = 100;
  my $numgraphs = 1;
  my $graphchecked = 1;
  my $textchecked = 0;
  my $refreshfreq = 10;
  print "<b>Resource Predictions and Measurements</b>";
  print "<table width = \"900\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"rpswebinterface.pl\">
          <table width = \"900\" border=1>
           <tr bgcolor=\"33ffcc\">
            <td width=\"100\">Buffer or Stream</td>
	    <td width=\"100\">Resource type</td>
	    <td width=\"100\">Resource</td>
	    <td width=\"100\">Clients</td>
	    <td width=\"100\">Depth</td>
	    <td width=\"100\">Number Graphs</td>
	    <td width=\"100\">Output</td>
            <td width=\"200\">Error</td>
	    <td width=\"100\"></td></tr>";
               
  print "  <tr bgcolor=\"33ff99\">";
  print "  <td><select name=\"bufferstream\" onChange=\"submitToCGI6()\">
           <option value=\"buffer\" SELECTED>buffer</option>
           <option value=\"stream\"> stream</option></select></td>";

  print "  <td><select name=\"resourcetype\" onChange=\"submitToCGI5()\">";
  if ($resourcetype eq "host load")
   {
    print "<option value=\"host load\" SELECTED>host load</option>
	   <option value=\"network bandwidth\"> network bandwidth</option>
           <option value=\"general resource\">general resource</option></select></td>";
   }
  elsif($resourcetype eq "network bandwidth")
   {
    print "<option value=\"network bandwidth\" SELECTED>network bandwidth</option>
	   <option value=\"host load\"> host load</option>
           <option value=\"general resource\">general resource</option></select></td>";
   }
  elsif($resourcetype eq "general resource")
   {
    print "<option value=\"general resource\" SELECTED>general resource</option>
           <option value=\"host load\"> host load</option>
           <option value=\"network bandwidth\"> network bandwidth</option></select></td>";
   }
  else
   {
    print "<option value=\"host load\" SELECTED>host load</option>
	   <option value=\"network bandwidth\"> network bandwidth</option>
           <option value=\"general resource\">general resource</option></select></td>";
   }
  print "<td><select name=\"resource\">";
  foreach(@resources)
   {
    if ($resource eq $_)
     {
      $resource = $_;
      print "<option value = \"$_\" SELECTED>$_</option>";
     } 
    else
     {
      print "<option value = \"$_\">$_</option>";
     }
   }

  print "       </select><br>
                <input type=button name=\"delete\" value=\"Delete\" onClick=\"submitToCGI2()\">
                <input type=button name=\"add\" value=\"Add\" onClick=\"submitToCGI3()\"></td>
            <td>";

  if ($cgi->param("predbufferclient"))
   {
    $predbufferchecked=1;
    print "<input type=\"checkbox\" name=\"predbufferclient\" value=\"predbufferclient\" CHECKED>predbufferclient<br>";
   }
  else
   {
    print "<input type=\"checkbox\" name=\"predbufferclient\" value=\"predbufferclient\">predbufferclient<br>";
   }
  if ($cgi->param("measurebufferclient"))
   {
    $measurebufferchecked=1;
    print "      <input type=\"checkbox\" name=\"measurebufferclient\" value=\"measurebufferclient\" CHECKED>"
                . "measurebufferclient<br></td>";
   }
  else
   {
    print "<input type=\"checkbox\" name=\"measurebufferclient\" value=\"measurebufferclient\" >measurebufferclient<br></td>";
   }

  if ($cgi->param("predbufferclientnum") && $cgi->param("predbufferclientnum")<=180)
   {
    $predbufferclientnum = $cgi->param("predbufferclientnum");
   }
  if ($cgi->param("measurebufferclientnum") && $cgi->param("measurebufferclientnum")<=180)
   {
    $measurebufferclientnum = $cgi->param("measurebufferclientnum");
   }
  if ($cgi->param("numgraphs"))
   {
    $numgraphs = $cgi->param("numgraphs");
   }
  print "   <td><input type=\"TEXT\" name=\"predbufferclientnum\" value=\"$predbufferclientnum\" size=\"3\"><br>
                <input type=\"TEXT\" name=\"measurebufferclientnum\" value=\"$measurebufferclientnum\" size=\"3\"></td>
            <td><input type=\"TEXT\" name=\"numgraphs\" value=\"$numgraphs\" size=\"2\"></td>";

  if($cgi->param("graph"))
   {
    $graphchecked = 1;
    print " <td><input type=\"checkbox\" name=\"graph\" value=\"graph\" CHECKED>Graph<br>";
   }
  else
   {
    $graphchecked=0;
    print " <td><input type=\"checkbox\" name=\"graph\" value=\"graph\">Graph<br>";
   }
  if($cgi->param("text"))
   {
    $textchecked = 1;
    print " <input type=\"checkbox\" name=\"text\" value=\"text\" CHECKED>Text<br></td>";
   }
  else
   {
    $textchecked=0;
    print " <input type=\"checkbox\" name=\"text\" value=\"text\">Text<br></td>";
   }
  if ($cgi->param("noerror"))
   {
    print "<td><input type=\"checkbox\" name=\"noerror\" value=\"noerror\" CHECKED>no error</td>";
    $noerror = 1;
   }
  else
   {
    print "<td><input type=\"checkbox\" name=\"noerror\" value=\"noerror\" >no error</td>";
   }
  print "<td><input type=\"button\" name=\"plot\" value=\"plot\" onClick=\"submitToCGI1()\"></td>";

  if ($cgi->param("refreshfreq"))
   {
    $refreshfreq= $cgi->param("refreshfreq");
   }
  print "</table>autorefresh<input name = \"autorefresh\" type=\"checkbox\" onClick=\"submitToCGI4()\"> every
		 <input type=\"textbox\" name = \"refreshfreq\" size=\"3\" value=\"$refreshfreq\"> seconds.</form>";
  print "</td><td width = \"50\"><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></table>";
  return ($predbufferchecked,
          $measurebufferchecked,
          $noerror,
          $predbufferclientnum,
          $measurebufferclientnum,
          $numgraphs,
          $graphchecked,
          $textchecked)
 }
