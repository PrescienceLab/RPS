#!/usr/local/bin/perl -w

package RPSHTTPInterface;

use strict;
use CGI;
use URI::Escape;

my $cgi = new CGI;
my $client = $cgi->param('client');
$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";

print $cgi->header();
#print "Content-Type: text/html\n\n";

# Most importantly
if( not $client || not is_valid_client($client) ) {
  error("There was no client or an incorrect client specified.");
}

# Handles redirections... doesn't run RPS.
if ($cgi->param('querybuilder') && $cgi->param('querybuilder') eq "Use the selected tool\'s query builder") {
  query_builder_redirect();
}

# Handles sourcebuilder errors.
my $source_string = undef;
if ($cgi->param('sourcebuilder')) {
  $source_string = source_builder();
}

# Specifies which level of detail form this request came from.
my $type = $cgi->param('type') || error("No type parameter");

my $cl;
if ($type eq "generic") {

  $cl = $cgi->param('args');
  if (is_buffered_client($client)) {
    run_buffer_command($cl);
  } else {
    $cgi->param('refresh', 2 );
    run_stream_command($cl);
  }
} elsif ($type eq "no-op") {
  # Do naaassssing, Lebowski
  if (not is_buffered_client($client)) {
    display_stream_command();
  } else {
    display_buffer_command();
  }


} elsif (is_buffered_client($client) && ($type eq "buffered" || $type eq "querybuilder")) {
  my $num_measures;
  ($num_measures= $cgi->param('num_measures')) || ($num_measures = 10);

  $cl = "$num_measures ";
  $cl .= source_builder();
  if ($client eq "predlatclient") {
    # Extra stuff.
    my $output_options;
    ($output_options = $cgi->param('output_options')) || ($output_options = "stats");
    $cl .= " $output_options";
  }

  run_buffer_command($cl);
} elsif (not is_buffered_client($client) && ($type eq "nonbuffered" || $type eq "querybuilder")) {
  $cl = source_builder();

  if (not $cl || $cl =="") {
    error("Command line wasn't set");
  }
  if ($client eq "pred_reqresp_client") {
    # Extra stuff.
    my $datafile;
    ($datafile = $cgi->param('datafile')) || query_builder_redirect();
    my $confidence_intervals;
    ($confidence_intervals = $cgi->param('confidence_intervals')) || ($confidence_intervals = "noconf");
    my $model;
    ($model = $cgi->param('confidence_intervals')) || ($model = "REFIT r");

    $cl .= " $datafile $confidence_intervals $model";
  }
  run_stream_command($cl);
} else {
  error("Invalid type of query $type with client $client");
}

exit;


sub run_stream_command {
  my $program = $client;
  my $args = shift;

  my @output;

  shell_safe($program, $args) || error("Security will not allow that to be executed.");

  open(comIN, "$program $args 2>&1 |")  ||
    error("Could not get pipe from RPS client.");

  my $num = 1;
  if ($program eq "predclient") {
    $num = 10;
  }

  my $rin = 0;
  my $rout;
  my $buffer = "";
  while(1) {
    # Read the first n times then quit.
    vec($rin, fileno(comIN), 1) = 1;
#    vec($rin, fileno(STDERR), 1) = 1;
#    vec($
    $rout = $rin;
    select($rout, undef, undef, undef);

    if (vec($rout, fileno(comIN), 1)) {
      sysread(comIN, $buffer, 1024);
      push @output, $buffer;
      if (--$num == 0 ) {
	last;
      }
    }
  }
  # Kill the process.
  (close comIN);  
  kill $$;

  display_stream_command(@output);
}

sub display_stream_command {
  my $refresh = $cgi->param('refresh');
  my @output = @_;
  my $program = $client;
  my $total_output;

  my $server_port = $cgi->server_port();
  my $server_and_port = $ENV{"HTTP_HOST"};
  my $script_name = $cgi->script_name();
  my $urlencodedstr;

  $server_and_port || ($server_and_port = "localhost:8000");

  if ($cgi->param('prev_results')) {
    $total_output = $cgi->param('prev_results');
  }
  
  foreach(@output) {
    $total_output .= $_;
  }

  $cgi->param('prev_results', $total_output);

  my $self_url = $cgi->self_url();
  my @thirds =  split(/\?/, $self_url);
  my $third1 = $thirds[1];

  # Get the <html> started.
  print "<html><head><title>Query results for $program</title>\n";
  if ($refresh) {
    print "<meta http-equiv=\"refresh\" content=\"$refresh; URL=http://$server_and_port";
    print "$script_name?$third1\">\n";
  }
  print "</head><body>\n";


  # Display common header stuff for stream clients.
#  open(FD, "stream_client_header.html");
#  while(<FD>) {
#    print;
#  }
#  close FD;


  print "<strong>$program results:</strong>";


  print "<form action=\"rpsquery.pl\" method=\"get\">\n";
  print "<input type=\"hidden\" name=\"type\" value=\"querybuilder\">\n";
  print "<input type=\"hidden\" name=\"client\" value=\"$program\">\n";
  # Output the data (previous data first, if any)
  print "<textarea name=\"prev_results\" cols=\"80\" rows=\"15\">\n";
  
  print $total_output;
  
  print "</textarea><br><br>\n";

  # Output the correct form for this client.
  print "Rerun $program:<br>\n";
  print_unbuilt_source();
  $refresh || ($refresh = "Refresh");
  print "<input type=\"text\" name=\"refresh\" value=\"$refresh\" size=\"7\">\n";

  print_extra_inputs();

  
  print " <input type=\"submit\" name=\"runquery\" value=\"Run Query\"></form><br>\n";
  if ($refresh) {
    $cgi->delete('refresh');
    $cgi->param('type', "no-op");
    $self_url = $cgi->self_url();
    @thirds =  split(/\?/, $self_url);
    
    $third1 = $thirds[1];
    
    print "<a href=\"rpsquery.pl?$third1\"><strong>Kill Refresh</strong></a><br><br>\n";
    $cgi->param('type', $type);
    $cgi->param('refresh', $refresh);
  }
#  open(FD, "$(program)_query_form.html");
#  while(<FD>) {
#    print;
#  }
#  close FD;

  open(FD, "dynamic_footer.lbi");
  while(<FD>) {
    print;
  }
  close FD;

  print "</body></html>\n";
}

sub run_buffer_command {
  my $args = shift;
  my $program = $client;

  shell_safe($program, $args) || error("Security will not allow that to be executed.");
  my @output = `$program $args 2>&1`;

  display_buffer_command(@output)

}

sub display_buffer_command {
  my $program = $client;
  my @output = @_;
  my $size = @output;

  print "<html><head><title>Query results for $program</title>\n";
  print "</head><body>\n";

  # Display common stuff for a buffer client
#  open(FD, "buffer_client_header.html");
#  while(<FD>) {
#    print;
#  }
#  close FD;

  # Output the data.
  print "<strong>$program results:</strong><br>\n";
  print "<form action=\"rpsquery.pl\" method=\"get\">\n";
  print "<input type=\"hidden\" name=\"client\" value=\"$program\">", 
        "<input type=\"hidden\" name=\"type\" value=\"querybuilder\">\n";

  print "<textarea name=\"prev_results\" cols=\"80\" rows=\"15\">\n";
  
  foreach(@output) {
    print;
    #print "\n";
  }
  
  print "</textarea><br>\n";
  
  # Output the correct form for this client.
  print "<br><strong>Rerun $program:</strong>";
  # Rebuild form
  print_unbuilt_source();
  my $num_measures;
  ($num_measures = $cgi->param('num_measures')) || ($num_measures = "# Measures");
  print " <input type=\"text\" name=\"num_measures\" value=\"$num_measures\" size=\"10\">\n";
  print_extra_inputs();
  print " <input type=\"submit\" name=\"runquery\" value=\"Run Query\"></form><br><br>\n";

  open(FD, "dynamic_footer.lbi");
  while(<FD>) {
    print;
  }
  close FD;

  print "</body></html>\n";
}

sub error {
  my $description = shift;

  print "<html><head><title>RPS Web Interface Error</title></head>\n";
  print "<body><h1>Error: $description</h1><br></body></html> \n";
  exit;
}

sub query_builder_redirect {
  if (is_valid_client($client) ) {
    # Spew redirect html
    print "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=${client}.html\"></head></html>\n"
  } else {
    error("Bad client name");
  }

  exit;
}

# BOOL
sub is_valid_client {
  my @client_tools = ("flowbwbufferclient",
		      "flowbwclient",
		      "loadbufferclient",
		      "loadclient",
		      "loadlatclient",
		      "measurebufferclient",
		      "measureclient",
		      "predbufferclient",
		      "predclient",
		      "predlatclient",
		      "pred_reqresp_client" );
  my $program = shift;

#  if ($_ in @client_tools) 
#    return 1;
#  else
#    return 0;

  foreach (@client_tools) {
    if ($_ eq $program) {
      return 1;
    }
  }

  return 0;
}

sub is_buffered_client {
  my @buffered_clients = ("flowbwbufferclient",
			  "loadbufferclient",
			  "loadlatclient",
			  "measurebufferclient",
			  "predbufferclient",
			  "predlatclient");
  my $client = shift;

  foreach(@buffered_clients) {
    if ($_ eq $client) {
      return 1;
    }
  }

  return 0;
}

sub shell_safe {
  foreach(@_) {
    if( m/[;><&\|]/ ) {
      return 0;
    }
  }
  return 1;
}

sub source_builder {
  my $proto = $cgi->param('streamsource');
  my $server = $cgi->param('server');
  my $port = $cgi->param('port');

  if (not $proto || not $server || not $port ) {
    error("Source building failed.");
    return "";
  }
  
  my $retval = "source:" . $cgi->param('streamsource') . ":";
  if ($proto eq "tcp") {
    $retval .= "$server:";
  }
  $retval .= $port;

  return $retval;
  
}

sub print_unbuilt_source {
  my $server = $cgi->param('server');
  my $port = $cgi->param('port');
  my $proto = $cgi->param('streamsource');

  $port || ($port = "Port");
  $server || ($server = "Server");

  print " <select name=\"streamsource\">";
  
  print " <option value=\"tcp\"";
  $proto && $proto eq "tcp" && print " selected";
  print ">TCP</option><option value=\"udp\"";
  $proto && $proto eq "udp" && print " selected";
  print ">UDP</option><option value=\"unix\"";
  $proto && $proto eq "unix" && print " selected";
  print ">UNIX</option></select>\n";

  print "<input type=\"text\" name=\"server\" value=\"$server\">";
  print "<input type=\"text\" name=\"port\" value=\"$port\" size=\"6\">";
  print "<input type=\"hidden\" name=\"sourcebuilder\" value=\"true\">\n";
}

sub print_extra_inputs {
  if ($client eq "pred_reqresp_client") {
    my $datafile = $cgi->param('datafile');
    $datafile || ($datafile = "Data file");
    my $confidence_intervals = $cgi->param('confidence_intervals');
    $confidence_intervals || ($confidence_intervals = "noconf");
    print " <input type=\"text\" name=\"datafile\" value=\"$datafile\">";
    print " <select name=\"confidence_intervals\">";
    if ($confidence_intervals eq "conf") {
      print " <option value=\"conf\" selected>Yes</option>";
      print " <option value=\"noconf\">No</option>";
    } else {
      print " <option value=\"conf\">Yes</option>";
      print " <option value=\"noconf\" selected>No</option>";
    }
    print "</select>\n";
  } elsif($client eq "predlatclient") {
    print " <select name=\"output_options\">";
    if ($cgi->param('output_options') eq "stats") {
      print " <option value=\"stats\" selected>Print Latency Stats</option>";
      print " <option value=\"dump\">Print Latency Measurements</option>";
    } else {
      print " <option value=\"stats\">Print Latency Stats</option>";
      print " <option value=\"dump\" selected>Print Latency Measurements</option>";      
    }
    print "</select>\n";
  }
}
