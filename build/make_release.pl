#!/usr/bin/env perl

#
# lhs is the package directory
# rhs, if it exists, is the toggle to turn off in the config files
#
%PackagesToRemove = ( "OLD" => "",
                      "Extra" => "",
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

print "Removing and turning off undistributed packages\n";
foreach $package (keys %PackagesToRemove) {
  print "Removing $package\n";
  `rm -rf $package`;
  if (!($PackagesToRemove{$package} eq "")) {
    print "Modifying configuration makefiles for $package\n";
    foreach $mc (split(/\s+/,`ls build/Makefile.conf.*`)) {
      fixfile($mc,
	      "$PackagesToRemove{$package}\\s*=\\s*YES",
	      "$PackagesToRemove{$package} = NO");
    }
  }
}

print "Turning off other packages.\n";
foreach $package (keys %PackagesDeactivate) {
  print "Turning off $package\n";
  if (!($PackagesToDeactivate{$package} eq "")) {
    print "Modifying configuration makefiles for $package\n";
    foreach $mc (split(/\s+/,`ls build/Makefile.conf.*`)) {
      fixfile($mc,
	      "$PackagesToDeactivate{$package}\\s*=\\s*YES",
	      "$PackagesToDeactivate{$package} = NO");
    }
  }
}


print "Fixing up RPS.dsw and .dsp files.\n";

fixfile("RPS.dsw","\n","\r\n");
foreach $dsp (split(/\s+/,`find . -name "*.dsp" -print`)) {
  print "Fixing $dsp\n";
  fixfile($dsp,"\n","\r\n");
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


sub fixfile {
  my ($infile, $searchre, $replre) = @_;
  my $outfile=$infile."re";
#  print "$infile, $searchre, $replre\n";
  open (IN, $infile);
  open (OUT, ">$outfile");
  while (<IN>) {
    s/$searchre/$replre/g;
    print OUT;
  }
  close(IN);
  close(OUT);
  `mv $outfile $infile`;
}

