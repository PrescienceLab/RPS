#!/usr/local/bin/perl5 -w


$CMD = "ps x";

open(FOO, "$CMD |");

while (<FOO>) {
    chomp;
    $line = $_;
    @fields = split;
    $cmd = $fields[4];
    if ($cmd =~ /loadserver/ || $cmd =~ /loadbuffer/ ) {
	$CMD = "kill $fields[0]";
	print STDERR "KILL $line\n";
	system "$CMD";
    }
}
