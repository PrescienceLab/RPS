#!/usr/local/bin/perl -w


use FileHandle;
use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;

my $db = rps_env->initENV("shoykhet","statqos","ORACLE","streamclients");

$db->RPSDBinit($ENV{"RPSDB_USER"},$ENV{"RPSDB_PASSWD"},$ENV{"RPSDB_PATH"});

my $cgi= new CGI();
my $resource = $cgi->param("resource");
my $resourcetype = $cgi->param("resourcetype");


if ($resourcetype eq "") {$resourcetype = "host load";}

my @resources = $db->RPSDBaddresses($resourcetype);



my $bufferdepth = 200;

if($cgi->param("bufferdepth"))
  {
   $bufferdepth = $cgi->param("bufferdepth");
  }


printHeading($bufferdepth);


my ($timetodie,
    $predchecked,
    $measurechecked,
    $predbufferclientnum,
    $measurebufferclientnum) 
     = printMainForm($bufferdepth, @resources);

my @syscalls = $db->RPSDBstreamsyscalls($resource,$resourcetype);

if ($measurechecked)
 {
  foreach(@syscalls)
   {
    if ($_ =~ m/measureclient/)
     {
      startMeasStream($_,$timetodie);
     }
   }
 }
elsif($predchecked)
 {
  foreach(@syscalls)
   {
    if ($_ =~ m/predclient/)
     {
      startPredStream($_,$timetodie,$bufferdepth);
     }
   }
 }


sub startPredStream
 {
  my ($syscall,$timetodie,$bufferdepth) = @_; 
  
  STDOUT->autoflush(1);
  print "Incoming Prediction Stream:";
  open(STREAM,"$syscall |");  

  #this just simulates a prediction server, for debugging purposes
  #open(STREAM,"perl fakepredserver.pl |");

  print "<form name=\"myForm\">";
  print "<textarea type=\"hidden\" name=\"output\" rows=\"20\" width = \"200\">";
  my $i = 1;
  while (<STREAM>) 
   {
    if ($_ =~ m/ModelInfo/ || $_ =~ m/PDQ/)
    {
     $i = 1;
    }
    else
     {
      my @output = split('\+',$_);
      shift(@output);
  
      my $textoutput = "";
  
      my @splitline = split(' ',$output[0]);
      $textoutput .= "$i $splitline[1]\n";
      $i++;

      print $textoutput; 
      $textoutput = "";
     }
    if(time() > $timetodie)
      {
       print "</textarea></form>";
       print $cgi->end_html();

       my $childid = $$ + 1;
       system("kill -9 $childid &");
       die "STREAM CLOSED";
      }
   }
 }
  

sub startMeasStream
 {
  my ($syscall,$timetodie) = @_; 
  
  STDOUT->autoflush(1);
  print "Incoming Measurement Stream:";
  open(STREAM,"$syscall |");
  print "<form name=\"myForm\">";
  print "<textarea type=\"hidden\" name=\"output\" rows=\"20\" width = \"100\">";
  my $i = 0;
  while (<STREAM>) 
   {
    my @output = split(' ',$_);
    print "$i $output[2]\n";
    $i++;
    if(time() > $timetodie)
      {
       print "</textarea></form>";
       print $cgi->end_html();

       my $childid = $$ + 1;
       system("kill -9 $childid &");
       die "STREAM CLOSED";
      }
   }
 }

sub printHeading
 {
  my ($bufferDepth) = @_;

  print "Content-Type: text/html\n\n";
  
  print "<head>".
         "<title>Stream</title>";
  print "<script language=javascript>
        var myArrayLength = $bufferDepth;
        var myArray = new Array(myArrayLength);
        var maxValue = 1;

        function updateBuffer() 
         {
              if (document.mainForm.client[0].checked)
               {
                updatePredBuffer();
               }
              else
               {
                updateMeasBuffer();
               }
         }

        function updatePredBuffer()
         {
            var myOutput = document.myForm.output.value;
            var tempArray = new Array(myOutput.length);
            tempArray = myOutput.split(\"\\n\");
            tempArray= tempArray.reverse();
            var firstval = tempArray[1];
            var minArray = new Array(3);
            minArray = firstval.split(\" \");
            var min = parseInt(minArray[0]);
            var secondval = tempArray[2];
            var minaheadArray = new Array(3);
            minaheadArray = secondval.split(\" \");
            var minahead = parseInt(minaheadArray[0]);
            var i = 3;
            var splitline = new Array(3);
            var tempArrayVal;
            while(min >= minahead && i < myArrayLength)
             {
              min = minahead;

              tempArrayVal = tempArray[i];
              
              splitline = tempArrayVal.split(\" \");
              minahead = parseInt(splitline[0]);
              i++;
             } 
            for(k = 0; k<myArrayLength; k++)
             {
              myArray[k] = \"\";
             }
            for(j = i-2; j>=0; j--)
             {
              myArray[j] = tempArray[j];
             }
            myArray=myArray.reverse();
            var outputstring = myArray.join(\"\");
            document.myForm.output.value = outputstring;
            updatePredGraph();
         }

        function updatePredGraph()
         {
          for(i = 0; i<myArrayLength; i++)
           {
            var value = myArray[i];
            var splitvalue = new Array(2);
            if (value != null)
             {
              tempvalue = value.substr(0,value.length-1);
              var splitdata = tempvalue.split(\" \");
              var height = splitdata[1];
              var jsCall = \"document.mainForm.image\" + i + \".height=300*height;\";
              eval(jsCall);    
             }
           }
          window.setTimeout(\"updatePredBuffer()\" , 1000); 
         } 

        function updateMeasBuffer()
         {
            var myOutput = document.myForm.output.value;
            var tempArray = new Array(myOutput.length);
            tempArray = myOutput.split(\"\\n\");
            tempArray = tempArray.reverse();
            for(i=myArrayLength-1; i >= 0 ; i--)
             {
              myArray[i] = tempArray[i];
             }
            myArray=myArray.reverse();
            var outputstring = myArray.join(\"\");
            document.myForm.output.value=outputstring;
            updateMeasGraph();    
         }
        function updateMeasGraph()
         {  
          for(i = 0; i<myArrayLength; i++)
           {
            var value = myArray[i];
            var splitvalue = new Array(2);
            if (value != null)
             {
              tempvalue = value.substr(0,value.length-1);
              var splitdata = tempvalue.split(\" \");
              var height = splitdata[1];
              var jsCall = \"document.mainForm.image\" + i + \".height=300*height;\";
              eval(jsCall);    
             }
           }
          window.setTimeout(\"updateMeasBuffer()\" , 1000); 
         }
       function submitToCGI1()
        {
         document.mainForm.action = \"stream.pl\";
         document.mainForm.submit();
        }
       function submitToCGI2()
        {
         document.mainForm.action = \"rpswebinterface.pl\";
         document.mainForm.submit();
        }
       function submitToCGI3()
        {
         document.mainForm.action = \"add.pl\";
         document.mainForm.submit();
        }
       function submitToCGI4()
        {
         document.mainForm.action = \"delete.pl\";
         document.mainForm.submit();
        }
      </script></head><body>";
 }


sub printMainForm
 {
  my ($bufferdepth,@resources) = @_;

  my $predchecked =0;
  my $measurechecked = 0;
  my $predbufferclientnum = 100;
  my $measurebufferclientnum = 100;
  my $timetodie = 600;

  print "<b>Resource Predictions and Measurements</b>";
  print "<table width = \"900\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"stream.pl\">
          <table width = \"900\" border=1>
           <tr bgcolor=\"33ffcc\">
            <td width=\"100\">Buffer or Stream</td>
	    <td width=\"100\">Resource type</td>
	    <td width=\"100\">Resource</td>
	    <td width=\"100\">Clients</td>
	    <td width=\"100\">Depth</td>
	    <td width=\"100\">Time to Die</td>
	    <td width=\"100\"></td></tr>";
               
  print "  <tr bgcolor=\"33ff99\">";
  print "  <td><select name=\"bufferstream\" onChange=\"submitToCGI2()\">
           <option value=\"stream\" SELECTED>stream</option>
           <option value=\"buffer\"> buffer</option></select></td>";

  print "  <td><select name=\"resourcetype\" onChange=\"submitToCGI1()\">";
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
                <input type=button name=\"delete\" value=\"Delete\" onClick=\"submitToCGI4()\">
                <input type=button name=\"add\" value=\"Add\" onClick=\"submitToCGI3()\"></td>";

 
  if ($cgi->param("client") eq "predclient")
   {
    $predchecked=1;
    print "   <td><input type = \"RADIO\" name=\"client\" value = \"predclient\" CHECKED>predclient<br>
	        <input type = \"RADIO\" name=\"client\" value = \"measureclient\">measureclient</td>"; 
   }

  else
   {
    $measurechecked=1;
    print "   <td><input type = \"RADIO\" name=\"client\" value = \"predclient\" >predclient<br>
	        <input type = \"RADIO\" name=\"client\" value = \"measureclient\" CHECKED>measureclient</td>";    
   }


  if ($cgi->param("timetodie"))
   {
    $timetodie = $cgi->param("timetodie");
   }

  print "   <td><input type=\"TEXT\" name=\"bufferdepth\" value=\"$bufferdepth\" size=\"3\"></td>
            <td><input type=\"TEXT\" name=\"timetodie\" value=\"$timetodie\" size=\"2\"></td>";

  print "<td><input type=\"button\" name=\"plot\" value=\"Update Stream\" onClick=\"submitToCGI1()\"></td>";

  print "</table>";
  print "</td><td width = \"50\"><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></table>";

  my $graphTableWidth = $bufferdepth * 2;
  my $outputTableWidth = $graphTableWidth + 100;

  print "<table width = \"$graphTableWidth\" border =\"0\" bordercolor = \"black\" bgcolor = \"black\">
          <td >";
  print "<img name=\"leftside\" src=\"images/leftside.BMP\"></img>";

  for(my $i = 0; $i < $bufferdepth ; $i++)
   {
    print"<img name = \"image$i\" src=\"images/box.BMP\" height=\"1\" width=\"2\"></img>"
   }
  print "</td></table><table>
         <td>
          <input type=\"button\" value =\"Graph Stream\" onClick=\"updateBuffer()\"><br>
         </td>";
  print "</table></form>";
  return (time() + $timetodie,
          $predchecked,
          $measurechecked,
          $predbufferclientnum,
          $measurebufferclientnum);
 }


