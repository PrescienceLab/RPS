#!/usr/bin/env perl
use RPS;
use Getopt::Long;

$#ARGV>=6 or die 
"Run text measurements through a prediction server and make the\n".
"predictions available via TCP streaming and request/response interface\n\n".
"usage: prediction_source.pl [--model='MANAGED ...'] numpred ctrl_port stream_port|none buffer_port|none numitems period text-generator\n\n".
"MANAGED ...    = a managed model. Default is \n".
"                  MANAGED 300 300 100 0.25 0.25 AR 16\n".
"numpred        = prediction horizon (steps ahead)\n".
"ctrl_port      = server port for configuring prediction\n".
"stream_port    = listening port number for streaming connections\n".
"buffer_port    = listening port number for request/response connections\n".
"numitems       = number of predictions to buffer\n".
"period         = period of input in us\n".
"text-generator = code to run to generate measurements\n".
RPSBanner();

$model = "MANAGED 300 300 100 0.25 0.25 AR 16";

&GetOptions("model=s"=>\$model ) ;

$numpred=shift;
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
$cmd .= " $numpred $model ";

if (!($buffer_port =~/n|N/)) {
  $cmd .= " | predbuffer $numitems source:stdio:stdin server:tcp:$buffer_port ";
}

print "Executing: ($cmd)\n";

system "($cmd)";



