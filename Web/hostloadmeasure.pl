#!/usr/bin/perl -w

$ENV{"RPS_DIR"} or die "Set RPS_DIR\n";

$sourcefile=$ENV{"RPS_DIR"}."/Web/measurebuffers.txt";

open(MB,"$sourcefile"); @mblist=<MB>; close(MB);


