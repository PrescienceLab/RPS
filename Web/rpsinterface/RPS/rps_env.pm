#!/usr/bin/env perl

package rps_env;

use strict;
use URI::Escape;


  
#
# Path to the RPS executables
#
# It must be possible to execute them as the apache user
#
$ENV{"PATH"} .= ":/virtuoso/home/pdinda/RPS/bin";


#
# Whether to use the TEXT database (in ./db)
# or an ORACLE database
#
$ENV{RPS_DB}="TEXT";


#
# Location of the text config files if you're using a text database
#
# The files streamclients and bufferclients must be readable and writable
# by the apache user
#
$ENV{RPS_TEXTDB_STEM}="/virtuoso/home/pdinda/RPS/web/db";


#
# Database logoninfo (needed only for Oracle)
#
$ENV{RPS_DBUSER}="pdinda";
$ENV{RPS_DBPASS}="lalalala";

#
# Oracle info in case you're using oracle
#
$ENV{ORACLE_HOME} = "/usr/home/oracle/oraInventory";
$ENV{ORACLE_BASE} = "/usr/home/oracle/oraInventory";
$ENV{ORACLE_SID}="GIS";


#
# Server and port where a pred_reqresp_server is running
#
#
$ENV{"RPS_PREDSERVER_HOST"}="virtuoso-32.cs.northwestern.edu";
$ENV{"RPS_PREDSERVER_PORT"}=7777;

#
# Server and port where a wavelet_reqresp_server is running
#
#
$ENV{"RPS_WAVELETSERVER_HOST"}="virtuoso-32.cs.northwestern.edu";
$ENV{"RPS_WAVELETSERVER_PORT"}=7778;

$ENV{"HOSTLOADPREDBUFFERPORT"}=5001;

#in general the InitENV call looks like:
#
#
#InitENV(table)
#
#

sub InitENV
    {
  my $self = shift;
  my $path = shift;
  my $db;

  $ENV{"RPSDB_PATH"} = $path;

  if ($ENV{"RPS_DB"} eq "ORACLE")
   {
    use RPS::rps_oracle_db;
    $db = new rps_oracle_db;
   }
  elsif ($ENV{"RPS_DB"} eq "TEXT")
   {
    $ENV{RPSDB_PATH}=$ENV{RPS_TEXTDB_STEM}."/$path";
    use RPS::rps_text_db;
    $db = new rps_text_db;
   }
  else
   {
    die "ENVIRONMENT VARIABLE RPS_DB (DATABASE TYPE) NOT SET!";
   }
  return $db;
 }
 
1;
