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

my $timefilename = $$;

printHeading();


my ($predbufferchecked,
    $measurebufferchecked,
    $noerror,
    $predbufferclientnum,
    $measurebufferclientnum,
    $numgraphs,
    $graphchecked,
    $textchecked) = printMainForm($resource);

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
  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource,$resourcetype);
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
  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource, $resourcetype);

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

  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource,$resourcetype);
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
  my $refreshcode = "";
  if ($cgi->param("playcontrol") ne "pause")
  {
   if ($cgi->param("refreshfreq"))
    {
     my $refreshfreq = $cgi->param("refreshfreq") * 1000;
     $refreshcode = "setTimeout(\"document.mainForm.submit()\",$refreshfreq);";
    }
   else
    {
     $refreshcode = "setTimeout(\"document.mainForm.submit()\",10000);";
    }
  }
  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head><title>RPS Web Interface</title>
              <script type=text/javascript>
		$refreshcode
               function submitToCGI1()
                {
                 document.mainForm.action = \"rpswebinterface.pl\";
                 document.mainForm.submit();
                }
               function switchToPlay()
                {
                 document.mainForm.action = \"autorefresh.pl\";
                 document.mainForm.playcontrol.value = \'play\';
                 document.mainForm.submit();
                }
               function switchToPause()
                {
                 document.mainForm.action = \"autorefresh.pl\";
                 document.mainForm.playcontrol.value = \'pause\';
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
  my $refreshfreq = 10;
  my $resourcetype = $cgi->param("resourcetype");
  print "<b>Resource Predictions and Measurements</b>";
  print "<table width = \"800\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"autorefresh.pl\">
          <table width = \"800\" border=1>
           <tr bgcolor=\"33ffcc\">
	    <td width=\"100\">Resource type</td>
	    <td width=\"100\">Resource</td>
	    <td width=\"100\">Clients</td>
	    <td width=\"100\">Depth</td>
	    <td width=\"100\">Number Graphs</td>
	    <td width=\"100\">Output</td>
            <td width=\"200\">Error</td>
	    <td width=\"100\"></td></tr>";
           
  print "  <tr bgcolor=\"FFFFDD\">
            <td><input type=\"hidden\" name=\"resourcetype\" value=\"$resourcetype\">$resourcetype</td>
            <td><input type=\"hidden\" name=\"resource\" value=\"$resource\">$resource<br> 
		<img src=\"images/delete.JPG\"></img><img src=\"images/add.JPG\"></img>";
  

  print"</td><td>";

  if ($cgi->param("predbufferclient"))
   {
    $predbufferchecked=1;
    print "<input type=\"hidden\" name=\"predbufferclient\" value=\"predbufferclient\" CHECKED><img src=\"images/checked.JPG\"></img>predbufferclient<br>";
   }
  else
   {
    print "<img src=\"images/unchecked.JPG\"></img>predbufferclient<br>";
   }
  if ($cgi->param("measurebufferclient"))
   {
    $measurebufferchecked=1;
    print "      <input type=\"hidden\" name=\"measurebufferclient\" value=\"measurebufferclient\" CHECKED>"
                . "<img src=\"images/checked.JPG\"></img>measurebufferclient</td>";
   }
  else
   {
    
    print "<img src=\"images/unchecked.JPG\"></img>measurebufferclient";
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
  print "   <td><input type=\"hidden\" name=\"predbufferclientnum\" value=\"$predbufferclientnum\">$predbufferclientnum<br>
                <input type=\"hidden\" name=\"measurebufferclientnum\" value=\"$measurebufferclientnum\">$measurebufferclientnum</td>
            <td><input type=\"hidden\" name=\"numgraphs\" value=\"$numgraphs\">$numgraphs</td>";

  if($cgi->param("graph"))
   {
    $graphchecked = 1;
    print " <td><input type=\"hidden\" name=\"graph\" value=\"graph\" CHECKED><img src =\"images/checked.JPG\"></img>Graph<br>";
   }
  else
   {
    $graphchecked=0;
    print " <td><img src =\"images/unchecked.JPG\"></img>Graph<br>";
   }
  if($cgi->param("text"))
   {
    $textchecked = 1;
    print " <input type=\"hidden\" name=\"text\" value=\"text\" CHECKED><img src =\"images/checked.JPG\"></img>Text<br></td>";
   }
  else
   {
    $textchecked=0;
    print " <img src =\"images/unchecked.JPG\"></img>Text<br></td>";
   }
  if($cgi->param("noerror"))
   {
    $noerror=1;
    print "<td><input type=\"hidden\" name=\"noerror\" value=\"noerror\"><img src=\"images/checked.JPG\"></img>no error</td>";
   }
  else
   {
   print "<td><img src=\"images/unchecked.JPG\"></img>no error</td>";
   }
  print "<td><input type=\"button\" name=\"plot\" value=\"plot\" onClick=\"submitToCGI1()\"></td>";

  if ($cgi->param("refreshfreq"))
   {
    $refreshfreq= $cgi->param("refreshfreq");
   }
  print "</table>autorefresh<input name = \"autorefresh\" type=\"checkbox\" onClick=\"submitToCGI1()\" CHECKED> every
		 <input type=\"textbox\" name = \"refreshfreq\" size=\"3\" value=\"$refreshfreq\"> seconds.";
  if ($cgi->param("playcontrol") eq "pause")
   {
    print "<input type=\"image\" src =\"images/play.JPG\" onClick=\"switchToPlay()\">";
    print "<input type=\"hidden\" name=\"playcontrol\" value=\"pause\">";

   }
  else
   {
    print "<input type=\"image\" src =\"images/pause.JPG\" onClick=\"switchToPause()\">";
    print "<input type=\"hidden\" name=\"playcontrol\" value=\"play\">";
   }
  print "<input type=\"image\" src=\"images/stop.JPG\" onClick=\"submitToCGI1()\">";
  print "</form>";
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
