#!clean_loadserver.pl


$CMD = "ps x";

open(FOO, "$CMD |");

while (<FOO>) {
    chomp;
    $line = $_;
    @fields = split;
    $cmd = $fields[4];
    if ($cmd =~ /loadserver/ || $cmd =~ /loadbuffer/ || $cmd =~ /load2measure/
         || $cmd =~ /measurebuffer/ ) {
	$CMD = "kill $fields[0]";
	print STDERR "KILL $line\n";
	system "$CMD";
    }
}
