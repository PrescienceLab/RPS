#!/usr/bin/perl -w

use HTTP::Daemon;
use HTTP::Status;
use IPC::Open2;
use FileHandle;

exists($ENV{"RPS_DIR"}) or die "Set RPS_DIR\n";

$dir=$ENV{"RPS_DIR"}."/Web";


$#ARGV==0 or die "webinterface.pl port\n";

$port=shift;


%GetHash = ( "/" => $dir."/top.html",
	     "/top.html" => $dir."/top.html",
	     "/oneoffpred.html" => $dir."/oneoffpred.html"
	   );

%PostHash = ( "/oneoffpred.pl" => $dir."/oneoffpred.pl"
	    );



$daemon = HTTP::Daemon->new(LocalPort=>$port) || die;
print "RPS web interface active at ", $daemon->url(), "\n";
while ($con = $daemon->accept()) {
  while ($req = $con->get_request()) {
    if ($req->method() eq 'GET') {
      if (exists($GetHash{$req->url()->path()})) {
	$con->send_file_response($GetHash{$req->url()->path()});
      } else {
	$con->send_error(RC_NOT_FOUND);
      }
    } elsif ($req->method() eq 'POST') {
      if (exists($PostHash{$req->url()->path()})) {
	if (not($child=open2(IN,OUT, $PostHash{$req->url()->path()}))) {
	  $con->send_error(RC_NOT_FOUND);
	  $con->close();
	  next;
	}
	print OUT $req->content();
	close(OUT);
	@reply=<IN>;
	close(IN);
	$reply=join("\n",@reply);
	$resp=HTTP::Response->new(200,"OK");
	$resp->content($reply);
	$con->send_response($resp);
      } else {
	$con->send_error(RC_NOT_FOUND);
      }
    } else {
      $con->send_error(RC_FORBIDDEN);
    }
  }
  $con->close;
  undef($con);
}
