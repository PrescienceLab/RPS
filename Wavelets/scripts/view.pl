#!/usr/bin/perl -w

use FileHandle;

$#ARGV==0 or die "view.pl file\n";

$file = shift;

open(G,"|gnuplot");
G->autoflush(1);

print G "plot \"$file\" with lines\n";

<STDIN>;

print G "quit\n";

exit;
