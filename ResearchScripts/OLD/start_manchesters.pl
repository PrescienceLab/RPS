#!/usr/local/bin/perl5 -w

$DIR = "/afs/cs/project/cmcl-pdinda-2/LoadMon";
$CMD = "perl $DIR/start_lpp_on.pl";

for ($i=1;$i<=8;$i++) { 
    system "$CMD manchester-$i unixpipeline.pl ALPHA DUX";
}

