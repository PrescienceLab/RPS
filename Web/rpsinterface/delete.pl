#!/usr/local/bin/perl -w


use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;


my $cgi;
$cgi = new CGI;

my $db;
my $bufferstream = $cgi->param("bufferstream");

if($bufferstream eq "stream")
 {
  $db = rps_env->initENV("shoykhet","statqos","ORACLE","streamclients");
 }
else
 {
  $db = rps_env->initENV("shoykhet","statqos","ORACLE","bufferclients");
 }
$db->RPSDBinit($ENV{"RPSDB_USER"},$ENV{"RPSDB_PASSWD"},$ENV{"RPSDB_PATH"});


my $resource = $cgi->param("resource");

if ($cgi->param("deleteresource"))
{
 $db->RPSDBdelete($cgi->param("deleteresource"));
}

my @resources = $db->RPSDBresources();
printHeading();
printMainForm(@resources);
print $cgi->end_html();

sub printHeading
 {
  print "Cache-Control: no-cache";
  print "Expires: Thu, 13 Mar 2003 07:12:13 GMT";
  print "Content-Type: text/html\n\n";

  print "<head>".
         "<title>Delete Resource</title>
          <script type=text/javascript>
           function submitToCGI1()
            {
             document.mainForm.action = \"delete.pl\";
             document.mainForm.submit();
            }
	   function submitToCGI2()
            {
             document.mainForm.action = \"add.pl\";
             document.mainForm.submit();
            }
           function submitToCGI3()
            {
             document.mainForm.action = \"rpswebinterface.pl\";
             document.mainForm.submit();
            }
	   function submitToCGI4()
	    {
	     location.replace(\"delete.pl\");
	    }
          </script>
         </head>";
 }

sub printMainForm
 {
  my (@resources) = @_;
  print "<b>Delete a Resource</b>";
  print "<table width = \"600\" border = 0><tr><td>";
  print "<form name=\"mainForm\" method=POST action =\"delete.pl\">
          <table width = \"600\" border=1>
           <tr bgcolor=\"33ffcc\">
            <td width=\"200\">Buffer or Stream</td>
	    <td width=\"100\">Resource</td>
           </tr>";

  print "<tr bgcolor=\"33ff99\">
          <td><select name=\"bufferstream\" onChange=\"submitToCGI1()\">";
  if ($bufferstream eq "stream")
   {
    print "<option value = \"stream\" SELECTED>stream</option>".
          "<option value = \"buffer\" > buffer</option></select></td>";
   }   
  else
   {
    print "<option value = \"buffer\" SELECTED>buffer</option>".
          "<option value = \"stream\" > stream</option></select></td>";
   }
    print "<td><select name=\"deleteresource\">";
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
  print " </select>";
  print "<input type=button name=\"Delete\" value=\"Delete\" onClick=\"submitToCGI1()\">";
  print "<input type=button name=\"Add\" value=\"Add\" onClick=\"submitToCGI2()\">";
  print "<input type=button name=\"Plot\" value=\"Go Plot\" onClick=\"submitToCGI3()\"></td></tr>";
  print "</table></form>";
  print "</td><td width = \"50\"><a href=\"home.htm\"><img src=\"images/home.JPG\"></img></a></td></table>";
 }
