#!/usr/bin/env perl


use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;

my @colors = ("x404040", "x0000ff", "xff0000", "xffa500", "x66cdaa", "xcdb5cd", "xdda0dd", "x000000", "xadd8e6","x9500d3");

rps_env->InitENV("streamclients");

my $cgi = new CGI();
my $name = "";

printHeading();

my ($file,
    $text,
    $daub,
    $levels,
    $transform,
    $dynamic,
    $block)  
     = printMainForm();



    if ($file)
     {
      $file=~m/^.*(\\|\/)(.*)/; 
      $name = $2;
      open(LOCAL, ">temp/$name") or die $!;
      while(<$file>) 
       {
        print LOCAL $_;
       }
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

if ($dynamic)
 {
  #dynamicwavelets code goes here
 }
else
 {
  if($block)
   {
    #static block code goes here
   }
  else
   {

  
    if($name)
     {
      my $measurements = formatMeasData($name); 
      graphStaticWavelet($measurements, $name);
     }
   }
 }


sub graphStaticWavelet
 {
  my($measurements, $name) = @_;
  my $outname = "temp/$name"."out";
  my @sfwtoutput = `sample_static_sfwt temp/$name $daub $levels $transform flat stdout 2>&1`;
  `sample_static_sfwt temp/$name $daub $levels $transform $outname flat`;

  print "Signal to Wavelet Transform:<br>";
  my @parsedsfwt = parseWavelet(@sfwtoutput, $levels);
  my $N = @parsedsfwt-1;
  print "<table>";
  for (my $i = $N; $i >=0; $i--)
   {
    if ($parsedsfwt[$i] ne "")
     {print "<tr>";
      print "<td width=\"20\">$i</td>";
      print "<td>";
      print "<img src=\"";
      print genGnuPlot2($i,$parsedsfwt[$i]);
      print "\"></img><br>";
      print "</td>\n";
      print "</tr>\n";
     }
   }
  print "</table>\n";
  print "Wavelet to Signal Transform<br>";
  if ($transform ne "TRANSFORM")
   {
    print "Can only reconstruct signal from wavelet using TRANSFORM.<br>";
   }
  else
   {
  my @srwtoutput = `sample_static_srwt $outname $daub $levels TRANSFORM stdout flat 2>&1`;
  print "<textarea name=\"reconstructed signal\" rows = \"20\" cols = \"60\" font = \"-3\">";
  my @splitmeasurements = split("\n",$measurements);
  my $length = @splitmeasurements;
  print "   Reconstructed signal      |      Original Measurements\n";
  for (my $j = 0; $j < $length ; $j++)
   {
    my @srwtsplit = split("\n",$srwtoutput[$j]);
    my $spacestring = "";
    for (my $k = 0; $k + length($srwtsplit[0]) < 30; $k++)
     {
      $spacestring .=" ";
     }
    
    print "$srwtsplit[0] $spacestring $splitmeasurements[$j]\n";
   }
#  foreach(@srwtoutput)
#   {print;}
  print "</textarea>";
   }
 }

print $cgi->end_html();

sub genGnuPlot2
 {
  my ($i,$outputdata) = @_;
  my $namebase = "gnuplot/$$";
  my $jpegfile = "$namebase".$i.".jpeg";
  my $gnuplotfile = $namebase . $i;

  my $xlabel;
  my $ylabel;
  my $xformat;
  my $yformat = "set format y \"\"\n";
 
  my $x = 600;
  my $y = 100;

  my $plotscript = "";

    my $filename = "$namebase".$i.".dat";
    open(MYOUTFILE, ">$filename");
    print MYOUTFILE $outputdata;
    close(MYOUTFILE);
    $plotscript.= "plot \'$filename\' using 1:2 title \'level $i\' with boxes\n";


  if($i == 0)
   {
    $ylabel = "\'values\'";
    $xlabel = "\'time\'";
    $xformat = "";
   }
  else
   {
    $ylabel = "\'\'";
    $xlabel = "\'\'";
    $xformat = "set format x \"\"\n";
   }
  
  open(MYOUTFILE, ">$gnuplotfile");
#
#
# Changed to use PNGs for portability -PAD
#
  print MYOUTFILE "set terminal png small transparent color " .
#  print MYOUTFILE "set terminal jpeg transparent small size $x,$y " .
  "xffffff x000000 xadd8e6 " .
  "$colors[$i] \n" .
  "set noborder \n" .
  $xformat.
  $yformat.
  "set output \"$jpegfile\" \n" .
  "set xlabel $xlabel \n" .
  "set ylabel $ylabel \n";
  print MYOUTFILE $plotscript;
  
  close(MYOUTFILE);

  system("gnuplot $gnuplotfile");

  return $jpegfile;
 }

sub genGnuPlot
 {
  my (@outputdata) = @_;
  my $namebase = "gnuplot/$$";
  my $jpegfile = "$namebase".".jpeg";
  my $gnuplotfile = $namebase;
  
  my $x = 600;
  my $y = 450;

  my $plotscript = "";
  my $i;
  my $asdf = @outputdata;
  my $levels = @outputdata;
  foreach($i = 0 ; $i < $levels - 1 ; $i ++)
   {
    my $filename = "$namebase".$i.".dat";
    open(MYOUTFILE, ">$filename");
    print MYOUTFILE $outputdata[$i];
    close(MYOUTFILE);
    $plotscript.= "\'$filename\' using 1:2 title \'level $i\',";
   }
  if ($levels > 0)
   {
    my $filename = "$namebase".$i.".dat";
    open(MYOUTFILE, ">$filename");
    print MYOUTFILE $outputdata[$i];
    close(MYOUTFILE);
    $plotscript.= "\'$filename\' using 1:2 title \'level $i\'\n";
    $plotscript = "plot " . $plotscript;
   }


  open(MYOUTFILE, ">$gnuplotfile");
#
#
# Changed to use PNGs for portability -PAD
#
  print MYOUTFILE "set terminal png small transparent color " .
#  print MYOUTFILE "set terminal jpeg transparent small size $x,$y " .
  "xffffff x000000 xadd8e6 " .
  "x9500d3 \n" .
  "set output \"$jpegfile\" \n" .
  "set xlabel \'time\' \n" .
  "set ylabel \'values\' \n";
  print MYOUTFILE $plotscript;
  
  close(MYOUTFILE);

  system("gnuplot $gnuplotfile");

  return $jpegfile;
 }
 

sub parseWavelet
 {
  my(@sfwtoutput,$levels) = @_;
  my @parsedsfwt;
  for(my $i = 0; $i < $levels+1 ; $i++)
   {
    $parsedsfwt[$i]="\n";
   }
  foreach(@sfwtoutput)
   {
    my @splitoutput = split(' ',$_);
    my $timestamp = $splitoutput[0];
    my $nlevels = $splitoutput[1];
    for(my $j = 0; $j<$nlevels; $j++)
     {
      $parsedsfwt[$j].="$timestamp $splitoutput[$j*2 + 3]\n";
     }
   }
  return @parsedsfwt;
}      

sub formatMeasData
 {
  my ($measfile) = @_;
  my $measurements = "";
  open(INFILE , "< temp/$measfile");
  while(<INFILE>)
   {
    $measurements .= "$_";
   }
  return $measurements;
 }

sub printHeading
 {
  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head>".
         "<title>Input Signal and Get a Wavelet</title>
           <script type=text/javascript>
           
           </script>
          </head>";
 }

sub printMainForm
 {
  my $file = $cgi->param("uploadedfile");
  my $text = $cgi->param("textinput");
  my $daub = $cgi->param("daub");
  my $levels=$cgi->param("levels");
  my $transform = $cgi->param("transform");
  my $dynamic = 0;
  my $block = 0;

  
  my @transforms = ("APPROX","DETAIL","TRANSFORM");


  print "<b>Input Signal and Get a Wavelet</b><br>";
  print "<table width = \"700\"><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"reqrespwavelets.pl\" enctype = \'multipart/form-data\'>
          <table width = \"800\" border=1>
           <tr bgcolor=\"33ffcc\">
            <td width=\"100\">Dynamic or Static</td>
            <td width=\"100\">Block or Sample</td>
	    <td width=\"100\">Input</td>
            <td width=\"100\">Wavelet Type</td>
            <td width=\"100\">Number of Levels</td>
            <td width=\"100\">Transform Type</td>
	    <td width=\"100\"></td>
	    </tr>";

  print "  <tr bgcolor=\"33ff99\">";
  if ($cgi->param("dynamic") eq "dynamic")
   {
    $dynamic = 1;
    print "<td><select name=\"dynamic\">
           <option value=\"dynamic\" CHECKED>dynamic</option>
           <option value=\"static\">static</option></select></td>";
   }
  else
   {
    print "<td><select name=\"dynamic\">
           <option value=\"static\" CHECKED>static</option>
           <option value=\"dynamic\">dynamic</option></select></td>";
   }
  
  if ($cgi->param("block") eq "block")
   {
    $block = 1;
    print "<td><select name = \"block\">
               <option value=\"block\" CHECKED>block</option>
               <option value=\"sample\">sample</option></select></td>";
   }
  else
   {
    
    print "<td><select name = \"block\">
               <option value=\"sample\" CHECKED>sample</option>
               <option value=\"block\">block</option></select></td>";
   }
  print "    <td>Input text in the form \"value , value , ...\"
                <input type=\"textbox\" name=\"textinput\" value=\"$text\" size=\"20\"><br>
                Or, Upload a file
                <input type=file name=\"uploadedfile\" value=\"$file\"></td>";
  print "  <td><input type=\"textbox\" name=\"daub\" value=\"$daub\"></td>";
  print "  <td><input type=\"textbox\" name=\"levels\" value=\"$levels\"></td>";
  print " <td><select name=\"transform\">";
  foreach(@transforms)
   {
    if ($transform eq $_)
     {
      print "<option value = \"$_\" SELECTED>$_</option>";
     }
    else
     {
      print "<option value = \"$_\">$_</option>";
     }
   }

  print "   <td><input type=submit name=\"submit\" value=\"submit\"></td>";
  print"   </tr></table></form>";
  print"</td><td><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></tr></table>";
  return ($file,
    $text,
    $daub,
    $levels,
    $transform,
    $dynamic,
    $block);
 }
