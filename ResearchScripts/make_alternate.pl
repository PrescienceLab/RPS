#!/usr/bin/env perl

$#ARGV==1 or die "usage: make_alternate.pl cmd1 cmd2\n";

while (1) {
  system $ARGV[0];
  system $ARGV[1];
}
