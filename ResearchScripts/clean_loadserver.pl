#!/usr/bin/env perl

use FileHandle;

autoflush STDOUT 1;

open(MC,"loadserver $ARGV[0] server:tcp:$ARGV[1] target:stdio:stdout | load2measure 0 source:stdio:stdin target:stdio:stdout | measureclient source:stdio:stdin |");


while (<MC>) { 
	@f=split; 
	print "$f[1]\t$f[2]\n";
}
