#!/usr/bin/env perl 
#arg 1 is a string to find
#arg 2 is a string to replace it with
#the rest of the args are files to do the replacement on
use File::Copy;

$temp_fn = '/tmp/.multi_repl';

$find = shift @ARGV;
# we need to armor the pattern
$find =~ s/(\W)/\\$1/g;
$replace = shift @ARGV;
while ($file = shift @ARGV) {
    if (not -w $file) {
        print "Skipping '$file' because it's not writable\n";
        next;
    }
    open(FILE, "<$file") || die "Could not open file '$file': $!";
    open(OUTFILE, ">$temp_fn") || die "Could not open tmp file: $!";
    $replacements = 0;
    while(defined($_ = <FILE>)) {
        #print;
        #print "sub '$find' for '$replace'\n";
        $replacements += ($_ =~ s/$find/$replace/go);
        #print;
        print OUTFILE;
    }
    close FILE;
    close OUTFILE;
    if ($replacements > 0) {
        copy($temp_fn, $file) || die "Could not copy the temp file back";
    }
}




