#!/usr/bin/env perl

use FileHandle;

$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";

system("gen_numbers.pl 1 | text2measure 10000 source:stdio:stdin connect:tcp:8888 target:udp:239.239.239.239:12000 &");
