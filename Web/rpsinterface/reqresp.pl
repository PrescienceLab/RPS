#!/usr/bin/env perl


use strict;
use CGI;
use URI::Escape;


$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";

my $server = "behemoth.cs.northwestern.edu";
my $serverport = "7777";


my $user = "shoykhet";
my $passwd = "statqos";
my $cgi = new CGI();
my $input = "";

printHeading();

my ($file,
    $text,
    $numahead,
    $conf,
    $modifier,
    $model,
    $graphchecked,
    $textchecked,) 
    = printMainForm();

my $name="";
my ($validmodel,$modelusage) = validModel($model);
my ($validmodifier,$modifierusage) = validModifier($modifier);

if ($validmodel && $validmodifier)
 {
  if ($file)
   {
    $file=~m/^.*(\\|\/)(.*)/; 
    $name = $2;
    open(LOCAL, ">temp/$name") or die $!;
    while(<$file>) 
     {
      print LOCAL $_;
     }
    close(LOCAL);
   }
  elsif ($text)
   { 
     my @text = split(',',$text);
     $name=$$;
     open(LOCAL, ">temp/$name");
     foreach(@text)
      {
       print LOCAL "$_ \n";
      }
     close(LOCAL);
   }
  
  if($name)
   {
    my $measurements = formatMeasData($name);

    if ($graphchecked && $textchecked)
     {
      graphAndPrintBoth($server,
                        $serverport,
                        $name,
                        $measurements,
                        $numahead,
                        $conf,
                        $modifier,
                        $model);
     }
    elsif($graphchecked)
     {
      graphBoth($server,
                $serverport,
                $name,
                $measurements,
                $numahead,
                $conf,
                $modifier,
                $model);
     }
    elsif($textchecked)
     {
      printBoth($server,
                $serverport,
                $name,
                $measurements,
                $numahead,
                $conf,
                $modifier,
                $model);
     }
   }
 }
else
 {
  if (!$validmodel)
   {
    print "<br><b>Invalid Model. $modelusage</b>";
   }
  if (!$validmodifier)
   {
    print "<br><b>Invalid Modifier. $modifierusage</b>";
   }
 }

print $cgi->end_html();

sub formatMeasData
 {
  my ($measfile) = @_;
  my $measurements = "";
  open(INFILE , "<temp/$measfile");
  my $i = 0;
  while(<INFILE>)
   {
    my @checkdouble = split(" ",$_);
    if ($checkdouble[1] ne "")
     {
      $measurements .= "$i $checkdouble[1]\n";
     }
    else
     {
      $measurements .= "$i $_";
     }
    $i--;
   }
  close(INFILE);
  return $measurements;
 }

sub validModifier
 {
  my ($modifier) = @_;
  if ($modifier)
   {
    my @output = `pred_reqresp_client 2>&1`;
    my @splitmodifier = split(' ',$modifier);
    foreach(@output)
     {
      my @splitoutput = split(' ', $_);
      my $numpars = @splitoutput;
      my $numinput = @splitmodifier;
      if($splitoutput[0] eq $splitmodifier[0] && $numpars != $numinput)
        {
         return(0, "Optional Modifier usage is: $_");
        }
     }
   }
  return (1,"");
 }

sub validModel
 {
  my ($model)= @_;
  my @output = `pred_reqresp_client 2>&1`;
  my @splitmodel = split(' ',$model);
  foreach(@output)
   {
    my @splitoutput = split(' ',$_);
    my $numpars = @splitoutput;
    my $numinput = @splitmodel;
    if($splitoutput[0] eq $splitmodel[0] && $numpars != $numinput)
      {
       return(0,"Model usage is: $_");
      }
   }
  return (1,"");
 }

sub graphAndPrintBoth
 {
  my ($server,
      $serverport,
      $name,
      $measurements,
      $numahead,
      $conf,
      $modifier,
      $model) = @_;

  my @graphs;
  my $predzerotime;
  
  my @output = `pred_reqresp_client client:tcp:$server:$serverport temp/$name $numahead $conf $modifier $model 2>&1`;
  ($predzerotime,@graphs) = genGnuPredData(@output);

  my $rows = 29;

  my ($preddata, $prederror) = separateDataAndError($graphs[0],$predzerotime);

  print "Prediction:";
  print "<img src =\"" . genGnuPlot(time(), 
                                    $measurements,
                                    $preddata,
                                    $prederror, 
                                    1, 
                                    $predzerotime,
                                    "Predictions",
                                    "") 
                       . "\"</img>\n";

  print "<textarea rows = $rows cols = 30>";
  print mergeDataAndError($graphs[0],$predzerotime);
  print "</textarea></br>";
 }

sub graphBoth
 {
  my ($server,
      $serverport,
      $name,
      $measurements,
      $numahead,
      $conf,
      $modifier,
      $model) = @_;

  my @graphs;
  my $predzerotime;
  
  my @output = `pred_reqresp_client client:tcp:$server:$serverport temp/$name $numahead $conf $modifier $model 2>&1`;
  ($predzerotime,@graphs) = genGnuPredData(@output);

  my $rows = 29;

  my ($preddata, $prederror) = separateDataAndError($graphs[0],$predzerotime);

  print "Prediction:";
  print "<img src =\"" . genGnuPlot(time(), 
                                    $measurements,
                                    $preddata,
                                    $prederror, 
                                    1, 
                                    $predzerotime,
                                    "Predictions",
                                    "") 
                       . "\"</img>\n";

 }

sub printBoth
 {
  my ($server,
      $serverport,
      $name,
      $measurements,
      $numahead,
      $conf,
      $modifier,
      $model) = @_;

  my @graphs;
  my $predzerotime;
  
  my @output = `pred_reqresp_client client:tcp:$server:$serverport temp/$name $numahead $conf $modifier $model 2>&1`;
  ($predzerotime,@graphs) = genGnuPredData(@output);

  my $rows = 29;

  print "<textarea rows = $rows cols = 30>";
  print "Measurements:\n";
  print "$measurements\n";
  print mergeDataAndError($graphs[0],$predzerotime);
  print "</textarea></br>";
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
 
  #try to make sure that if more than one graph is requested we can fit two
  #graphs side by side in an average window.

  if ($numgraphs > 1 && $x < 300)
   {
    $x = 300;
    $y = 225;
   }
  #generate three plots out of the data (data, uppererror and lower error)
  if ($preddata ne "" && $measdata ne "")
   {
    open(MYOUTFILE, ">gnuplot/P$datafile.dat");
    print MYOUTFILE $preddata;
    close(MYOUTFILE);
    open(MYOUTFILE, ">gnuplot/M$datafile.dat");
    print MYOUTFILE $measdata;
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
  elsif ($measdata ne "")
   {
    open(MYOUTFILE, ">gnuplot/M$datafile.dat");
    print MYOUTFILE $measdata;
    close(MYOUTFILE);
    $plotscript = "plot \'gnuplot/M$datafile.dat\' using 1:2 title \'Measurements\' with lines\n";
   }

  open(MYOUTFILE, ">gnuplot/$gnuplotfile");
  print MYOUTFILE "set terminal jpeg transparent small size $x,$y " .
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
  my $xbase = parseTime($output[1]);
    
  #get the length of each prediction vector that's being fed to us
  my $N = parseN($output[1]);

  foreach (@output)
   {
    if($index > 3)
     {
      my @templine = split(' ',$_);
     
      if ($index - 3 <= $numahead)
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
      if ($index >= $N + 3)
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
    for (my $i = 0 ; $i < @data ; $i++)
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
   }
   if ($preddata)
   {
   return($preddata,"");
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


sub printHeading
 {
  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head>".
         "<title>Input Signal and Get a Prediction</title>
           <script type=text/javascript>
           
           </script>
          </head>";
 }

sub printMainForm
 {
  my $file = $cgi->param("uploadedfile");
  my $text = $cgi->param("textinput");
  my $numahead = $cgi->param("numahead");
  my $conf = $cgi->param("conf");
  my $modifier=$cgi->param("optionalmod");
  my $model = $cgi->param("model");
  my $graphchecked = $cgi->param("graph");
  my $textchecked  = $cgi->param("text");


  print "<b>Input Signal and Get a Prediction</b><br>";
  print "<table width = \"700\"><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"reqresp.pl\" enctype = \'multipart/form-data\'>
          <table width = \"800\" border=1>
           <tr bgcolor=\"33ffcc\">
	    <td width=\"100\">Input</td>
            <td width=\"100\">Depth of Prediction</td>
            <td width=\"100\">Confidence Interval</td>
            <td width=\"100\">Optional Modifier</td>
            <td width=\"200\">Model</td>
            <td width=\"200\">Output</td>
	    <td width=\"100\"></td>
	    </tr>";

  print "  <tr bgcolor=\"33ff99\">
            <td>Input text in the form \"value , value , ...\"
                <input type=\"textbox\" name=\"textinput\" value=\"$text\" size=\"20\"><br>
                Or, Upload a file
                <input type=file name=\"uploadedfile\" value=\"$file\"></td>";
  print "  <td><input type=\"textbox\" name=\"numahead\" value=\"$numahead\"></td>";
  if ($conf eq "conf")
   {
     print "  <td><input type=\"radio\" name=\"conf\" value=\"conf\" CHECKED>Yes
           <br><input type=\"radio\" name=\"conf\" value=\"noconf\">No</td>";
   }
  else
   {
     print "  <td><input type=\"radio\" name=\"conf\" value=\"conf\">Yes
           <br><input type=\"radio\" name=\"conf\" value=\"noconf\" CHECKED>No</td>";
   }
  print "  <td><input type=\"textbox\" name=\"optionalmod\" value=\"$modifier\" size=\"12\"></td>";
  print "   <td><input type=\"textbox\" name=\"model\" value=\"$model\" size=\"15\">
                <a onClick = \"window.open(\'help.pl\')\"><img src = \"images/help.JPG\"></img></a></td>";
  if ($graphchecked)
   {
    print "   <td><input type=\"checkbox\" name=\"graph\" CHECKED>graph<br>";
   }
  else
   {
    print "   <td><input type=\"checkbox\" name=\"graph\">graph<br>";
   }
  if ($textchecked)
   {
    print "   <input type=\"checkbox\" name=\"text\" CHECKED>text</td>";
   }
  else
   {
    print "   <input type=\"checkbox\" name=\"text\">text</td>";
   }
 
  print "   <td><input type=submit name=\"submit\" value=\"submit\"></td>";
  print"   </tr></table></form>";
  print"</td><td><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></tr></table>";
  return ($file,
    $text,
    $numahead,
    $conf,
    $modifier,
    $model,
    $graphchecked,
    $textchecked);
 }
