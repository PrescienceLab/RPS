#!/usr/bin/env perl

use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;

my $db = rps_env->InitENV("bufferclients");

$db->RPSDBinit($ENV{"RPSDB_USER"},$ENV{"RPSDB_PASSWD"},$ENV{"RPSDB_PATH"});

my $numgraphs = 1;
my $measurebufferclientnum = 100;
my $predbufferclientnum = 100;

my $pid = $$;
my $timefilename = $pid;

my $user = "shoykhet";
my $passwd = "statqos";
my $cgi = new CGI;

#my $resource = $cgi->param("resource");
my @selectedresources =  $cgi->param("resources");

my @resources = $db->RPSDBaddresses("host load");

printHeading();

my ($runningtime ,
    $confidenceinterval,
    $graphchecked)
     = printMainForm(@resources);
if ($selectedresources[0])
 {
  my @htmloutput = formatOutput($runningtime,$confidenceinterval,@selectedresources);
  for(my $i = 0; $i < @htmloutput ; $i++)
   {
    print $htmloutput[$i];
    if ($graphchecked)
     {
      my @splitoutput = split('run on ',$htmloutput[$i]);
      my @selected = split(',',$splitoutput[1]);
      
      graphData($selected[0],
		 1 ,
		 $measurebufferclientnum,
		 $predbufferclientnum,
		 1,
		 1,
		 $timefilename . $i,
		 $i);
     }
   }
 }

print $cgi->end_html();

sub formatOutput
 {
  my($runningtime,$confidence,@resources) = @_;
  my @htmloutput;
  foreach(@resources)
  {
   my @returnedoutput = `test_rta $runningtime $confidenceinterval $_ 2>&1`;


   my @splitoutput = split(':',$returnedoutput[0]);
   my @splitestimation = split('] \(',$splitoutput[1]);
   my @interval = split(',',$splitestimation[0]);
   my $lowinterval = substr($interval[0],2,length($interval[0]));
   my $highinterval = $interval[1];
   my $estimate = substr($splitestimation[1],0,length($splitestimation[1])-2);

   push(@htmloutput,"A $runningtime second task will take an estimated <b>$estimate</b> seconds to run on $_,
                  <br> and is between <b>$lowinterval</b> and <b>$highinterval</b> seconds at a $confidence confidence level <br><br>");
  }
  return sort(@htmloutput);
 }

sub assoc
 {
  my ($element, @array) = @_;
  foreach(@array)
   {
    if ($element eq $_)
     {
      return 1;
     }
   }
  return 0;
 }


sub graphData
 {
  my ($resource, 
      $numgraphs, 
      $measurebufferclientnum, 
      $predbufferclientnum, 
      $predbufferchecked, 
      $measurebufferchecked,
      $timefilename,
      $j) = @_;

  my @systemcalls;
  @systemcalls = $db->RPSDBbuffersyscalls($numgraphs, $measurebufferclientnum, $resource,"host load");
  graphBoth($timefilename,$j, @systemcalls);
 
 }

sub graphBoth
 {
  my ($timefilename, $j , @systemcalls) = @_;
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



    my ($preddata, $prederror) = separateDataAndError($graphs[0],$zerotime);
    print "<img src =\"" . genGnuPlot($timefilename . 
				      $j, 
				      $measureData , 
				      $preddata,
                                      $prederror, 
				      1, 
				      $zerotime) 
	   . "\"</img><br>\n";

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
  my $x = 300;
  my $y = 225;
  my $plotscript = "";


  #the data comes in with the x-axis on the unix time scale
  #i.e. (12341243 firstvalue 12341244 secondvalue ...), we want
  #to normalize these values around $zerotime (the first value obtained
  #from the measurebufferclient)


  my $normalizemeasdata = normalize($measdata, $zerotime);
 

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
  "set output \"gnuplot/$jpegfile.jpeg\" \n" .
  "set xlabel \'time\' \n" .
  "set ylabel \'values\' \n";
  print MYOUTFILE $plotscript;
  
  close(MYOUTFILE);

  system("gnuplot gnuplot/$gnuplotfile");

  return "gnuplot/" . $jpegfile . ".jpeg";
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
              </script>
         </head>";
 }

sub printMainForm
 {
  my (@resources) = @_;
  my $runningtime;
  my $confidenceinterval;
  my $graphchecked;
  my $output;
  
  print "<b>Process Running Time Prediction</b>";
  print "<table width = \"600\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"test_rta.pl\">
          <table width = \"800\" border=1>
           <tr bgcolor=\"33ffcc\">
	    <td width=\"100\">Resource</td>
	    <td width=\"200\">Running Time</td>
	    <td width=\"100\">Confidence Interval</td>
	    <td width=\"300\">Output</td>
	    <td width=\"100\"></td></tr>";
               
  print "  <tr bgcolor=\"33ff99\">";
  print "<td><select name=\"resources\" MULTIPLE SIZE=\"5\">";
  foreach(@resources)
   {
    if (assoc($_,@selectedresources) == 1)
     {
      print "<option value = \"$_\" SELECTED>$_</option>";
     } 
    else
     {
      print "<option value = \"$_\">$_</option>";
     }
   }
  print "       </select><br>";
  if ($cgi->param("runningtime") ne "")
   {
    $runningtime = $cgi->param("runningtime");
    print "<td><input type=\"textbox\" name=\"runningtime\" value=\"$runningtime\" size=\"3\"></td>";
   }
  else
   {
    $runningtime = "10";
    print "<td><input type=\"textbox\" name=\"runningtime\" value=\"$runningtime\" size=\"3\"></td>";
   }
  if ($cgi->param("confidenceinterval") ne ""
      && $cgi->param("confidenceinterval") > 0
      && $cgi->param("confidenceinterval") < 1)
   {
    $confidenceinterval = $cgi->param("confidenceinterval");
    print "<td><input type=\"confidenceinterval\" name=\"confidenceinterval\" value=\"$confidenceinterval\" size=\"3\"></td>";
   }
  else
   {
    $confidenceinterval = ".95";
    print "<td><input type=\"confidenceinterval\" name=\"confidenceinterval\" value=\"$confidenceinterval\" size=\"3\"></td>";
   }
  if ($cgi->param("graphchecked") ne "")
  {
   $graphchecked = 1;
   print "<td><input type=\"checkbox\" value=\"Graph\" name=\"graphchecked\" CHECKED>Measurement/Prediction Graph</td>";
  }
  else
  {
   $graphchecked = 0;
   print "<td><input type=\"checkbox\" value=\"Graph\" name=\"graphchecked\">Measurement/Prediction Graph</td>";
  }
  print "<td><input type=\"submit\" value=\"Predict\" method=\"post\"></td>";
  print "</tr></table></form>";
  print "</td><td width = \"50\"><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></table>";
  return ($runningtime,$confidenceinterval,$graphchecked);
 }
