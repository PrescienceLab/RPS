#!/usr/bin/env perl

$DIR = "/afs/cs/project/cmcl-pdinda-2/LoadMon";
$CMD = "perl $DIR/kill_lpp_on.pl";

for ($i=1;$i<=8;$i++) { 
    system "$CMD manchester-$i";
}

