#!/usr/bin/env perl

use strict;
use CGI;
use URI::Escape;


$ENV{ORACLE_HOME} = "/usr/home/oracle/oraInventory";
$ENV{ORACLE_BASE} = "/usr/home/oracle/oraInventory";
$ENV{ORACLE_SID}="GIS";
$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";

my $cgi = new CGI;

print "Content-Type: text/html\n\n";

print "<head>".
      "<title>Request Response Manual</title></head>";

my @output = `pred_reqresp_client 2>&1`;

foreach(@output)
 {
  print $_ . "<br>";
 }



print $cgi->end_html();

