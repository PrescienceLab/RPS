#!/usr/local/bin/perl -w


use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;


my $cgi = new CGI();
my $bufferstream = $cgi->param("bufferstream");

my $db;

#in general the initEnv call looks like: 
#initEnv([dbuser],[dbpasswd],[TEXT | ORACLE], [textpath | oracle table name])
#in this case, if a text database is used, the initENV call looks like:
#rps_env->initENV("shoykhet","statqos","TEXT","db/streamclients.txt") or bufferclients.txt
#if an oracle database is used:
#rps_env->initENV("shoykhet","statqos","ORACLE","streamclients") or bufferclients

if ($bufferstream eq "stream")
 {
  $db = rps_env->initENV("shoykhet","statqos","ORACLE","streamclients");
 }
else
 {
  $db = rps_env->initENV("shoykhet","statqos","ORACLE","bufferclients");
 }

$db->RPSDBinit($ENV{"RPSDB_USER"},$ENV{"RPSDB_PASSWD"},$ENV{"RPSDB_PATH"});


my $resourcetype = $cgi->param("resourcetype");

my $client = $cgi->param("client");
my $protocol = $cgi->param("protocol");
my $address = $cgi->param("address");
my $port = $cgi->param("port");

printHeading();

if ($cgi->param("onchange") eq "0")
 {
  if(formFilled())
   {
    if(clientValid())
     { 
      $db->RPSDBadd($resourcetype,$client,$protocol,$address,$port);
      print "Resource added.<br>";
     }
    else
     {
      print "Error: No such resource is available: $resourcetype $client on $protocol:$address:$port<br>";
     }
   }
  elsif($resourcetype)
   {
    print "Error: You need to fill the form completely<br>";
   }
 }

printMainForm();

print $cgi->end_html();

$db->RPSDBshut();

sub printHeading
 {
  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head>".
         "<title>Add Resource</title>
           <script type=text/javascript>
            function submitToCGI1()
             {
              document.mainForm.action = \"add.pl\";
              document.mainForm.submit();
             }
            function submitToCGI2()
	     {
	      location.replace(\"delete.pl\");
	     }
	    function submitToCGI3()
       	     {
       	      location.replace(\"rpswebinterface.pl\");
       	     }
            function submitToCGI4()
             {
              document.mainForm.onchange.value=\"1\";
              document.mainForm.action =\"add.pl\";
              document.mainForm.submit();
             }
           </script>
          </head>";
 }

sub printMainForm
 {
  print "<b>Add a Resource</b>";
  print "<table width = \"600\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"add.pl\">
          <table width = \"700\" border=1>
           <tr bgcolor=\"33ffcc\">
	    <td width=\"100\">Resource Type</td>
            <td width=\"100\">Buffer or Stream</td>
	    <td width=\"100\">Clients</td>
	    <td width=\"100\">Protocol</td>
	    <td width=\"100\">Address</td>
	    <td width=\"100\">Port</td>
	    <td width=\"100\"></td></tr>";

  print "  <tr bgcolor=\"33ff99\">";
  if ($bufferstream eq "stream")
   {
    print "   <td><select name=\"bufferstream\" onChange=\"submitToCGI4()\">
                  <option value=\"stream\" SELECTED>stream</option>
                  <option value=\"buffer\" >buffer</option></select>
                  <input type=\"hidden\" name=\"onchange\" value=\"0\"></td>\n";
   }
  else
   {
    print "   <td><select name=\"bufferstream\" onChange=\"submitToCGI4()\">
                  <option value=\"buffer\" SELECTED>buffer</option>
                  <option value=\"stream\" >stream</option></select>
                  <input type=\"hidden\" name=\"onchange\" value=\"0\"></td>\n";
   }

  print "   <td><select name=\"resourcetype\">";
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
  
  if ($bufferstream eq "stream")
   {
    print "   <td><input type = \"RADIO\" name=\"client\" value = \"predclient\" CHECKED>predclient<br>
	        <input type = \"RADIO\" name=\"client\" value = \"measureclient\">measureclient</td>";                   
   }
  else
  {
  print "   <td><input type = \"RADIO\" name=\"client\" value = \"predbufferclient\" CHECKED>predbufferclient<br>
	        <input type = \"RADIO\" name=\"client\" value = \"measurebufferclient\">measurebufferclient</td>";                
  }
  print "   <td><select name=\"protocol\">";
  print "        <option value = \"tcp\" SELECTED>tcp</option>\n";
  print "        <option value = \"udp\" > udp </option>\n";
  print "       </select></td>";
  print "   <td><input type=\"TEXT\" name=\"address\" value=\"$address\" size=\"14\"></td>\n";
  print "   <td><input type=\"TEXT\" name=\"port\" value=\"$port\" size=\"5\"></td>\n";
 
  print"    <td><input type=button name=\"add\" value=\"Add\" onClick=\"submitToCGI1()\">
	        <input type=button name=\"delete\" value=\"Delete\" onClick=\"submitToCGI2()\">
                <input type=button name=\"plot\" value=\"Go Plot\" onClick=\"submitToCGI3()\"></td>";
  print"   </tr></table></form>";
  print "</td><td width = \"50\"><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></table>";
 }

sub formFilled
 {
  return ($resourcetype 
	  && $bufferstream
	  && $client
	  && $protocol
	  && $address
	  && $port);
 }


sub clientValid
 {
  my $systemcall  = "$client 1 client:$protocol:$address:$port";
  my @output = `$systemcall 2>&1`;
  if ($output[0] =~ m/can\'t/  || $output[0] =~ m/failed/ || $output[0] eq "")
   {
    return 0;
   }
  else
   {
    return 1;
   }
 }
