#!/usr/bin/env perl
use RPS;

$usage=
"Make wavelet-transformed text measurements available via TCP streaming\n".
"multicast streaming, and TCP request/response interface\n\n".
"usage: wavelet_measurement_source.pl stream_port|none buffer_port|none\n".
"       multicast_addr:port|none wavelet_type num_levels numitems\n".
"       period text-generator\n\n".
"stream_port    = listening port number for streaming connections\n".
"buffer_port    = listening port number for request/response connections\n".
"multicast_addr:port = multicast address to send data to\n".
"wavelet_type   = type of wavelet to use (DAUB2 ... DAUB20)\n".
"num_levels     = number of levels to use in transform\n".
"numitems       = number of measurements to buffer\n".
"period         = period of input in us\n".
"text-generator = code to run to generate measurements\n".
"               one (value) or two (timestamp value) columns\n".RPSBanner();

$#ARGV>=7 or die $usage;

$stream_port = shift;
$buffer_port = shift;
$multicast = shift;
$wavelet_type =shift;
$numlevels = shift;
$numitems = shift;
$period = shift;

$code = join(' ',@ARGV);

$cmd = $code." | text2measure $period source:stdio:stdin target:stdio:stdout | wavelet_streaming_server $wavelet_type $numlevels transform source:stdio:stdin";
if (!($stream_port =~/n|N/)) {
  $cmd .= " connect:tcp:$stream_port ";
}
if (!($multicast =~/n|N/)) {
  $cmd .= " target:udp:$multicast ";
}
if (!($buffer_port =~/n|N/)) {
  $cmd .= " target:stdio:stdout ";
  $cmd .= " | wavelet_buffer $numitems source:stdio:stdin server:tcp:$buffer_port ";
}

print "Executing: ($cmd) \n";

system "($cmd)";



