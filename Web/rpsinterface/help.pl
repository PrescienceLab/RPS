#!/usr/bin/env perl

use strict;
use CGI;
use URI::Escape;
use RPS::rps_env;

rps_env->InitENV("foo");


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

