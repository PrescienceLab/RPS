#!/usr/bin/env perl

use Getopt::Long;

$#ARGV>=9 or die 
"Run text measurements through a prediction server and make the\n".
"measuremetns and predictions available via TCP streaming \n".
"and request/response interface\n\n".
"usage: measurement_and_prediction_source.pl [--model='MANAGED ...'] measurement_period measurement_stream_port|none measurement_buffer_port|none measurement_numitems prediction_numahead prediction_ctrl_port prediction_stream_port|none prediction_buffer_port|none prediction_numitems text-generator\n\n".
"MANAGED ...    = a managed model. Default is \n".
"                  MANAGED 300 300 100 0.25 0.25 AR 16\n".
"measurement_period      = period (in us) for measurements\n".
"measurement_stream_port = listening port for streaming measurements\n".
"measurement_buffer_port = listening port for request/response connections\n".
"measurement_numitems    = number of measurements to buffer\n".
"numahead                = prediction horizon (steps ahead)\n".
"prediction_ctrl_port    = server port for configuring prediction\n".
"prediction_stream_port  = listening port number for streaming connections\n".
"prediction_buffer_port  = listening port number for request/response connections\n".
"prediction_numitems     = number of predictions to buffer\n".
"text-generator          = code to run to generate measurements\n".
"                           one (value) or two (timestamp value) columns\n\n".
"RPS: Resource Prediction System Toolkit\n".
"---------------------------------------\n\n".
"Copyright (c) 1999-2002 by Peter A. Dinda\n".
"Use subject to license (\$RPS_DIR/LICENSE)\n\n".
"http://www.cs.northwestern.edu/~RPS\n".
"rps-help\@cs.northwestern.edu\n";

$model = "MANAGED 300 300 100 0.25 0.25 AR 16";

&GetOptions("model=s"=>\$model ) ;

$measurement_period = shift;
$measurement_stream_port=shift;
$measurement_buffer_port=shift;
$measurement_numitems=shift;
$numahead=shift;
$prediction_ctrl_port = shift;
$prediction_stream_port = shift;
$prediction_buffer_port = shift;
$prediction_numitems = shift;
$code = join(' ',@ARGV);

$cmd = $code." | text2measure $measurement_period source:stdio:stdin target:stdio:stdout";
if (!($measurement_stream_port =~/n|N/)) {
  $cmd .= " connect:tcp:$measurement_stream_port ";
}
if (!($measurement_buffer_port =~/n|N/)) {
  $cmd .= " | measurebuffer $measurement_numitems source:stdio:stdin server:tcp:$measurement_buffer_port target:stdio:stdout ";
}

$cmd .= " | managed_predserver source:stdio:stdin server:tcp:$prediction_ctrl_port ";
if (!($prediction_stream_port =~/n|N/)) {
  $cmd .= " connect:tcp:$prediction_stream_port ";
}
if (!($prediction_buffer_port =~/n|N/)) {
  $cmd .= " target:stdio:stdout ";
}
$cmd .= " $numahead $model ";

if (!($prediction_buffer_port =~/n|N/)) {
  $cmd .= " | predbuffer $prediction_numitems source:stdio:stdin server:tcp:$prediction_buffer_port ";
}

print "Executing: ($cmd)\n";

system "($cmd)";



