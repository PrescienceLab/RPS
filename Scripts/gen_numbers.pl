#!/usr/bin/env perl

use RPS;

$usage=
"Generate numbers for playing with RPS\n\n".
"usage: gen_numbers.pl sample_rate alternate period \n".
"                      count|random|sin freq|square freq |saw freq|file\n".RPSBanner();

$#ARGV>=1 or die $usage;

$|=1;

$time=shift;
$time=1.0/$time;

$tag=join(" ",@ARGV);
$freq=1;
$file="";
$acount=-1;
$curalt=0;

@alternatelist=("random","sin","square","saw");

if ($tag=~/alternate\s+(.*)/) { 
  $curalt=0;
  $type=$alternatelist[$curalt];
  $alternatecount=$1;
} elsif ($tag=~/random/) { 
  $type="random";
} elsif ($tag=~/sin\s+(.*)/) { 
  $type="sin";
  $freq=$1;
} elsif ($tag=~/square\s+(.*)/) { 
  $type="square";
  $freq=$1;
} elsif ($tag=~/saw\s+(.*)/) { 
  $type="saw";
  $freq=$1;
} else {
  $type="file";
  $file=$tag;
  open(IN,$file) or die "Can't open $file\n";
}

for ($i=0;;$i++){
  $t=$i*$time;
  if ($type eq "random") { 
    $val=rand();
  }
  if ($type eq "sin") {
    $val=sin($freq*$t);
  }
  if ($type eq "square") {
    $val= ($t/$freq - int($t/$freq))  > 0.5 ? 1.0 : 0.0;
  }
  if ($type eq "saw") { 
    $val = ($t/$freq - int($t/$freq));
    if ($val <0.5) {
      $val=$val*2;
    } else {
      $val=(1.0-$val)*2;
    }
  }

  if ($alternatecount>0 && $i%$alternatecount==0) {
    $type=$alternatelist[(++$curalt)%($#alternatelist+1)];
  }
  print "$val\n";
  select(undef,undef,undef,$time);
}

