#!/usr/bin/env perl

$DIR = "/afs/cs/project/cmcl-pdinda-2/LoadMon";
$DIR =~ s/\//\\\//g;

#print STDERR "$DIR";


$CMD = "ps x";

#print STDERR "$CMD\n";

open(FOO, "$CMD |");

while (<FOO>) {
    chomp;
    $line = $_;
    @fields = split;
    $cmd = $fields[4];
    if ($cmd =~ /loadserver/ ||
	$cmd =~ /loadbuffer/ ||
	$cmd =~ /load2measure/ ||
	$cmd =~ /measurebuffer/ ||
	$cmd =~ /predserver/ ||
	$cmd =~ /predserver_core/ ||
	$cmd =~ /predbuffer/ ||
        $cmd =~ /evalfit/ ) {

	$CMD = "kill $fields[0]";
	print STDERR "KILL $line\n";
	system "$CMD";
    }
}
