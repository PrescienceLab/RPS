#!/usr/local/bin/perl -w

use dataaccess;
use sched_dataaccess;

@approaches = ("random", 
	       "measure", 
	       "mean", 
	       "last",
	       "ar16" );

@measures = ("avgfracinci", 
	     "avgfracinciwhenpossible", 
	     "avgnumpossible");


$#ARGV == 2 or die "usage: sched_gen_perhost_comparisons.pl database table measure";
$database = $ARGV[0];
$table = $ARGV[1];
$measure=$ARGV[2];

foreach $approach (@approaches) { 
