#!/usr/bin/env perl

use Getopt::Long;

$#ARGV>=5 or die 
"Run text measurements through a prediction server and make the\n".
"predictions available via TCP streaming and request/response interface\n\n".
"usage: prediction_source.pl [--model='MANAGED ...'] ctrl_port stream_port|none buffer_port|none numitems period text-generator\n\n".
"MANAGED ...    = a managed model. Default is \n".
"                  MANAGED 300 300 100 0.25 0.25 AR 16\n".
"ctrl_port      = server port for configuring prediction\n".
"stream_port    = listening port number for streaming connections\n".
"buffer_port    = listening port number for request/response connections\n".
"numitems       = number of predictions to buffer\n".
"period         = period of input in ns\n".
"text-generator = code to run to generate measurements\n".
"               one (value) or two (timestamp value) columns\n\n".
"RPS: Resource Prediction System Toolkit\n".
"---------------------------------------\n\n".
"Copyright (c) 1999-2002 by Peter A. Dinda\n".
"Use subject to license (\$RPS_DIR/LICENSE)\n\n".
"http://www.cs.northwestern.edu/~RPS\n".
"rps-help@cs.northwestern.edu\n";

$model = "MANAGED 300 300 100 0.25 0.25 AR 16";

&GetOptions("model=s"=>\$model ) ;

$ctrl_port = shift;
$stream_port = shift;
$buffer_port = shift;
$numitems = shift;
$period = shift;
$code = join(' ',@ARGV);

$cmd = $code." | text2measure $period source:stdio:stdin target:stdio:stdout";
$cmd .= " | managed_predserver source:stdio:stdin server:tcp:$ctrl_port ";

if (!($stream_port =~/n|N/)) {
  $cmd .= " connect:tcp:$stream_port ";
}
if (!($buffer_port =~/n|N/)) {
  $cmd .= " target:stdio:stdout ";
}
$cmd .= " $model ";

if (!($buffer_port =~/n|N/)) {
  $cmd .= " | predbuffer $numitems source:stdio:stdin server:tcp:$buffer_port ";
}

print "Executing: ($cmd)\n";

system "($cmd)";



