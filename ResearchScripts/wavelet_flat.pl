#!/usr/bin/env perl


while (<STDIN>) {
	/index=(\d+).*level=(\d+).*value=(.*)\)/;
	print "$2\t$1\t$3\n";
}
