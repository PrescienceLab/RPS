#!/usr/bin/env perl

#
# lhs is the package directory
# rhs, if it exists, is the toggle to turn off in the config files
#
%PackagesToRemove = ( "Extra" => "",
                      "ResearchScripts" => "",
                      "ResearchTools" => "HAVE_RESEARCHTOOLS");

#
#
#
%PackagesToDeactivate = ();


$tempdir="RPSRELEASETEMP";

$usage = "make_release.pl name [extra_cvs_checkout_arg]*\n";

$#ARGV>=0 or die $usage;

$name=$ARGV[0];

$cvsargs="";
if ($#ARGV>0) {
  $cvsargs.=join(" ",@ARGV[1..$#ARGV]);
}

`mkdir $tempdir` ;
chdir $tempdir;

print "Checking out copy from cvs\n";

`(export CVSROOT=:ext:pdinda\@skysaw.cs.nwu.edu:/home/RPS/CVS_RPS; export CVS_RSH=ssh; cvs co $cvsargs RPS-development)`;

chdir "RPS-development";

print "Removing CVS directories\n";
`rm -rf \`find . -name "CVS" -type d -print\``;

print "Removing non-distributed packages\n";
foreach $package (keys %PackagesToRemove) {
  print "Removing $package\n";
  `rm -rf $package`;
}

chdir "..";

rename "RPS-development", $name;

print "Creating tarball\n";
`tar cvfz $name.tgz $name`;

rename "$name.tgz", "../$name.tgz";

chdir "..";

print "Cleaning up.\n";
`rm -rf $tempdir`;

print "Done.\n";


