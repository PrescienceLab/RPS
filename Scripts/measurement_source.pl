#!/usr/bin/env perl

use RPS;

$#ARGV>=4 or die 
"Make text measurements available via TCP streaming and \n".
"request/response interface\n\n".
"usage: measurement_source.pl stream_port|none buffer_port|none numitems period text-generator\n\n".
"stream_port    = listening port number for streaming connections\n".
"buffer_port    = listening port number for request/response connections\n".
"numitems       = number of measurements to buffer\n".
"period         = period of input in us\n".
"text-generator = code to run to generate measurements\n".
"               one (value) or two (timestamp value) columns\n\n".
RPSBanner();

$stream_port = shift;
$buffer_port = shift;
$numitems = shift;
$period = shift;
$code = join(' ',@ARGV);

$cmd = $code." | text2measure $period source:stdio:stdin ";
if (!($stream_port =~/n|N/)) {
  $cmd .= " connect:tcp:$stream_port ";
}
if (!($buffer_port =~/n|N/)) {
  $cmd .= " target:stdio:stdout ";
  $cmd .= " | measurebuffer $numitems source:stdio:stdin server:tcp:$buffer_port ";
}

print "Executing: ($cmd)\n";

system "($cmd)";



