#!/usr/bin/perl -w

use rpsweb;
use HTTP::Daemon;
use HTTP::Status;
use IPC::Open2;
use FileHandle;

exists($ENV{"RPS_DIR"}) or die "Set RPS_DIR\n";

$dir=$ENV{"RPS_DIR"}."/Web";


$#ARGV==0 or die "webinterface.pl port\n";

$port=shift;



%WorkMap = ( "/" => \&RPSHandleTop,
	     "/top" => \&RPSHandleTop,
	     "/plab.gif" =>\&RPSGetFile,
	     "/rps.gif" => \&RPSGetFile,
	     "/resmeasure" =>\&RPSHandleResMeasure,
	     "/respred" => \&RPSHandleResPred,
	     "/apppred" => \&RPSHandleAppPred,
	     "/adapt" => \&RPSHandleAA,
	     "/oneoffpred" => \&RPSHandleUnimplemented,
	     "/any" => \&RPSHandleUnimplemented,
	     "/mtta" => \&RPSHandleUnimplemented,
	     "/rta" => \&RPSHandleRTA,
	     "/rtaq" => \&RPSHandleRTAQuery
	   );


%GetHash = ( "/" => $dir."/top.html",
	     "/top.html" => $dir."/top.html",
	     "/oneoffpred.html" => $dir."/oneoffpred.html",
	     "/any.pl" => $dir."/test.pl"
	   );

%PostHash = ( "oneoffpred.pl" => $dir."/oneoffpred.pl",
	      "hostloadmeasure.pl" => $dir."/hostloadmeasure.pl",
              "hostloadget.pl" => $dir."/hostloadget.pl",
	    );



$daemon = HTTP::Daemon->new(LocalPort=>$port) || die;
print "RPS web interface active at ", $daemon->url(), "\n";
while ($con = $daemon->accept()) {
  while ($req = $con->get_request()) {
    print $req->url()->path(), "\n";
    if (exists($WorkMap{$req->url()->path()})) {
      my $content = $WorkMap{$req->url()->path()}->($req);
      my $resp=HTTP::Response->new(200,"OK");
      $resp->content($content);
      $con->send_response($resp);
    } else {
      $con->send_error(RC_NOT_FOUND);
    }
    $con->close;
  }
}
      
      

sub SendFromSubProcess {
  my ($exec, $input, $con) = @_;
  my $content;
  my $child;

  if (not($child=open2(IN,OUT, $exec))) {
    print "hereh";
    $con->send_error(RC_NOT_FOUND); 
    $con->close(); 
  } else { 
    print OUT $input;
    close(OUT);
    my @reply=<IN>;
    close(IN);
    my $reply=join("\n",@reply);
    my $resp=HTTP::Response->new(200,"OK");
    $resp->content($reply);
    $con->send_response($resp);
  }
}
    


#     if ($req->method() eq 'GET') {
#       print $req->url()->path(), "\n";
#       if (exists($GetHash{$req->url()->path()})) {
# 	if ($req->url()->path =~/.*\.pl$/) {
# 	  my $exec=$GetHash{$req->url()->path()};
# 	  SendFromSubProcess($exec,$req->content(),$con);
# 	} else {
# 	  $con->send_file_response($GetHash{$req->url()->path()});
# 	}
#       } else {
# 	$con->send_error(RC_NOT_FOUND);
#       }
#     } elsif ($req->method() eq 'POST') {
#       if (exists($PostHash{$req->url()->path()})) {
# 	my $exec=$PostHash{$req->url()->path()};
# 	SendFromSubProcess($exec,$req->content(),$con);
#       } else {
# 	$con->send_error(RC_NOT_FOUND);
#       }
#     } else {
#       $con->send_error(RC_FORBIDDEN);
#     }
#      }
#      $con->close;
#    undef($con);
#}
  
