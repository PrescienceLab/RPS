#!/usr/bin/env perl


$CMD = "ps x";

open(FOO, "$CMD |");

while (<FOO>) {
    chomp;
    $line = $_;
    @fields = split;
    $cmd = $fields[4];
    if ($cmd =~ /hostloadpred/ || $cmd =~ /predbuffer/ ) {
	$CMD = "kill $fields[0]";
	print STDERR "KILL $line\n";
	system "$CMD";
    }
}
