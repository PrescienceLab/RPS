#!/usr/local/bin/perl -w

package rps_env;

use strict;
use URI::Escape;

sub initENV
 {
  my $self = shift;
  my ($user,$passwd,$dbtype, $path) = @_;
  my $db;
  $ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";


  $ENV{ORACLE_HOME} = "/usr/home/oracle/oraInventory";
  $ENV{ORACLE_BASE} = "/usr/home/oracle/oraInventory";
  $ENV{ORACLE_SID}="GIS";
  $ENV{"HOSTLOADPREDBUFFERPORT"}="5001";

  $ENV{"RPSDB_USER"} = $user;
  $ENV{"RPSDB_PASSWD"} = $passwd;
  $ENV{"RPSDB_PATH"} = $path;
  $ENV{"RPS_DB"} = $dbtype;


  if ($ENV{"RPS_DB"} eq "ORACLE")
   {
    use RPS::rps_oracle_db;
    $db = new rps_oracle_db;
   }
  elsif ($ENV{"RPS_DB"} eq "TEXT")
   {
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
