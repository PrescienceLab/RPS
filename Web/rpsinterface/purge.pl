#!/usr/bin/env perl


use strict;
use CGI;
use URI::Escape;
use Getopt::Long;


$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";


my $usage = "purge.pl [--threshold=threshold]";

my $threshold = 60;
&GetOptions(("threshold=i"=>\$threshold) ) ;



my $age = time();
my $mytime = time();
while(1)
 {
  my $dirtoget="gnuplot/";
  opendir(IMD, $dirtoget) || die("Cannot open directory");
  my @thefiles= readdir(IMD);
  closedir(IMD);
  foreach(@thefiles)
   {
    $mytime = time();
    if($_ ne ".." && $_ ne ".")
     {
      $age = (stat("gnuplot/" . $_))[9];
      if ($mytime - $age > $threshold)
       {
        unlink("gnuplot/" . $_);
       }
     }
   }
  $dirtoget="temp/";
  opendir(IMD, $dirtoget) || die("Cannot open directory");
  @thefiles= readdir(IMD);
  closedir(IMD);
  foreach(@thefiles)
   {
    $mytime = time();
    if($_ ne ".." && $_ ne ".")
     {
      $age = (stat("temp/" . $_))[9];
      if ($mytime - $age > $threshold)
       {
        unlink("temp/" . $_);
       }
     }
   }
  sleep($threshold);
 }
  
