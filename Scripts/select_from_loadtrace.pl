#!clean_loadserver.pl

if ($#ARGV!=2) { 
    print STDERR "usage: select_from_loadtrace.pl start:end source_trace target_trace\n";
}


($start,$end) = split(/:/,$ARGV[0]);

$sourcefile = $ARGV[1];
$destfile = $ARGV[2];




