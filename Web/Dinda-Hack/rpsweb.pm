package rpsweb;

use Exporter;
use FileHandle;
use IPC::Open2;

@ISA= qw(Exporter);
@EXPORT = qw(
	     RPSHandleUnimplemented
	     RPSGetFile
	     RPSHandleTop
	     RPSHandleResMeasure
	     RPSHandleResPred
	     RPSHandleAppPred
	     RPSHandleAA
	     RPSHandleRTA
	     RPSHandleRTAQuery
	     RPSHandleRequestPage
	    );


my @availlist;
my $rpsdir;
my $webdir;
my $weboutdir;
my $conffile;

$ENV{"RPS_DIR"} or die "set RPS_DIR\n";
$rpsdir=$ENV{"RPS_DIR"};
$webdir=$rpsdir."/Web";
$conffile=$webdir."/config";
open(IN,$conffile);
while (<IN>) {
  if (!/^\s*#.*$/ && !/^\s*$/) {
    my ($type, $basetype, $interface, $name, $endpoint) = split(/,/);
    push @availlist, [$type, $basetype, $interface, $name, $endpoint];
  }
}

return 1;


sub RPSMakeRequestPage {
  my ($title, $type, $script, $note) = @_;

my $out= <<END1;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <title>$title</title>
  </head>

  <body>
    <h1>$title</h1>

    $note

    <form method="POST" action="$script">
      <input type="hidden" name="$type">
      <select name="source">
END1
;
  my ($ref,$name,$endpoint);
  foreach $ref (@availlist) { 
    if ($type eq "ANY") {
      $out.= "<option>".join(",",@{$ref})."\n";
    } elsif ($ref->[0] eq $type) {
      $out.= "<option>".$ref->[1]."\n";
    }
  }
  $out.=<<END2;
      </select>
      <input type="submit" name="Submit">
      <input type="reset" name="reset">
     </form>
   </body>
  </html>
END2
;

    return $out;
}

sub RPSGetFile {
  my $req=shift;
  my $resp="";
  my $chunk;
  open (IN, $webdir.$req->url()->path());
  binmode IN;
  while ($chunk=sysread(IN,$chunk,1024)) {
    $resp.=$chunk;
  }
  close(IN);
  return $resp;
}

sub RPSHandleTop {
  my $req=shift;
  return <<END;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <title>RPS Web Interface</title>
  </head>

  <body>
    <h1>RPS Web Interface</h1>

    Please choose from among the following functions.<p>

    <menu>
      <li> <a href="adapt">Adaptation Advisors</a>
      <li> <a href="apppred">Application-level Performance Predictions</a>
      <li> <a href="respred">Resource Signal Predictions</a>
      <li> <a href="resmeasure">Resource Signal Measurements</a>
      <li> <a href="oneoffpred">One-Off Predictions</a>
      <li> <a href="any">Any</a>
    </menu>
   <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
  </body>
</html>
END
;
;
}


sub RPSHandleResMeasure {
return <<END;
<html>
  <title>RPS Resource Signal Measurements</title>
    <body>
      <h1>RPS Resource Signal Measurements</h1>
	<menu>
	  <li> Unix load measurements (<a href="hostloadmeasurebuf">buffered</a> <a href="hostloadmeasurestream">streaming</a>)
	  <li> Windows load measurements (<a href="wtmeasurebuf">buffered</a> <a href="wtmeasurestream">streaming</a>)
	  <li> Network bandwidth measurements (<a href="netbwmeasurebuf">buffered</a> <a href="netbwmeasurestream">streaming</a>)
	  <li> Network latency measurements (<a href="netlatmeasurebuf">buffered</a> <a href="netlatmeasurestream">streaming</a>)
       </menu>
    <hr>
   </body>
   <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
</html>
END
;
}

sub RPSHandleResPred {
return <<END;
<html>
  <title>RPS Resource Signal Predictions</title>
    <body>
      <h1>RPS Resource Signal Predictions</h1>
	<menu>
	  <li> Unix load predictions (<a href="hostloadpredbuf">buffered</a> <a href="hostloadpredstream">streaming</a>)
	  <li> Windows load predictions (<a href="wtpredbuf">buffered</a> <a href="wtpredstream">streaming</a>)
	  <li> Network bandwidth predictions (<a href="netbwpredbuf">buffered</a> <a href="netbwpredstream">streaming</a>)
	  <li> Network latency predictions (<a href="netlatpredbuf">buffered</a> <a href="netlatpredstream">streaming</a>)
       </menu>
    <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
   </body>
</html>
END
;
}

sub RPSHandleAppPred {
return <<END;
<html>
  <title>RPS Application Level Performance Advisors</title>
    <body>
      <h1>RPS Application Level Performance Advisors</h1>
	<menu>
	  <li> RTA: Running Time Advisor (<a href="rta">reqresp</a>)
          <li> MTTA: Message Transfer Time Advisor (<a href="mtta">reqresp</a>)
       </menu>
   <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
   </body>
</html>
END
;
}

sub RPSHandleAA {
return <<END;
<html>
  <title>RPS Adaptation Advisors</title>
    <body>
      <h1>RPS Adaptation Advisors</h1>
	<menu>
	  <li> RTA: Running Time Advisor (<a href="rta">reqresp</a>)
          <li> MTTA: Message Transfer Time Advisor (<a href="mtta">reqresp</a>)
       </menu>
    <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
   </body>
</html>
END
;
}


sub RPSHandleUnimplemented {
return <<END;
<html>
  <title>RPS Unimplemented Feature</title>
    <body>
      <h1>RPS Unimplemented Feature</h1>
      Sorry, that feature has not yet been implemented in the web interace.
    <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
   </body>
</html>
END
;
}

sub RPSHandleRTA {
my $out=<<END;
<html>
  <title>RPS Running Time Advisor</title>
    <body>
      <h1>RPS Running Time Advisor</h1>
      <form method="POST" action="rtaq"> 
         <input type="hidden" name="rta">
         <table> 
           <tr><td>Task Size (seconds) </td> <td> <input type="text" name="size" value="1.0"></td></tr>
           <tr><td>Confidence Level </td> <td><input type="text" name="conf" value="0.95"></td></tr>
           <tr><td>Host</td><td><select name="host">
END
;
my ($ref,$name,$endpoint);
foreach $ref (@availlist) { 
  if ($ref->[0] eq "PREDICTION" && $ref->[1] eq "HOSTLOAD" && $ref->[2] eq "BUFFER") {
#    $out.= "<option>".join(",",@{$ref})."\n";
    $out.= "<option>".$ref->[3].",".$ref->[4]."\n";
  }
}
$out.=<<END2;
      </select> </td></tr>
     <tr><td></td><td>
      <input type="submit" name="Submit">
      <input type="reset" name="reset">
        </td></tr>
      </table>
     </form>
    <hr>
   <a href="http://www.cs.northwestern.edu/~RPS"><img src="rps.gif"> RPS</a><br>
   a tool of the <br>
   <a href="http://plab.cs.northwestern.edu"><img src="plab.gif">Prescience Lab</a>
   </body>
  </html>
END2
;
    return $out;
}


sub InputToHash {
  my $input=shift;
  my %hash;
  my @pairs;
  my $pair;

  @pairs=split(/&/,$input);
  foreach $pair (@pairs) { 
    my ($name, $value) = split(/=/,$pair);
    $name =~ tr/+/ /;
    $name =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $name =~ tr/\0//d;
    
    $value =~ tr/+/ /;
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $value =~ tr/\0//d;
    $hash{$name}=$value;
  }
  return %hash;
}

sub RPSHandleRTAQuery {
  my $req=shift;
  my %hash = InputToHash($req->content());
  my ($host,$endpoint)=split(",",$hash{"host"});
  my $size=$hash{"size"};
  my $conf=$hash{"conf"};

  my $result = `test_rta $size $conf $host`;

my $out=<<END;
<html>
  <title>RPS Running Time Advisor Response</title>
    <body>
      <h1>RPS Running Time Advisor Response</h1>
      You are asking about a $size second task on $host ($endpoint) with confidence $conf.  The result was <p><p>

<pre>
$result
</pre>
</body>
</html>
END
;

  return $out;
}

