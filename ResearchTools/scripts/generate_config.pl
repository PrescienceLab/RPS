#!/usr/bin/perl -w

$#ARGV>=2 or die "usage: generate_config waveletnum numlevels delay [predictorinfo] \n";

$waveletnum=shift; 
$numlevels=shift;
$delay=shift;
if ($#ARGV>=0) {
  $pred=join(" ",@ARGV);
} else {
  $pred="default";
}


$n=2*($waveletnum+1);
$l=$numlevels-1;
$k=$delay;

print "# $numlevels levels\n$numlevels\n";
print "# wavelet type\n$waveletnum\n";

open(SWD, "setup_wavelet_delay $n $l $k |");
while (<SWD>) {
  if (/^d\[(\d+)\]=(\-*\d+)\s+/) {
    $i=$1; $d=-$2;
    $line="$d ";
    if ($d>0) {
      if ($pred eq "default") {
	$line.="await 25 ar 16";
      } else {
	$line.=$pred;
	}
    } else {
      $line.="delay";
    }
    print "$i $line\n";
    if ($i==$l-1) {
      $i++;
      print "$i $line\n"; 
    }
  }
}

