#!/usr/bin/perl -w

$ssh="ssh1";

@hosts=(
"sunlab-01",
"sunlab-02",
"sunlab-03",
"sunlab-04",
"sunlab-05",
"sunlab-06",
"sunlab-07",
"sunlab-08",
"sunlab-09",
"sunlab-10",
"sunlab-11",
"sunlab-12",
"sunlab-13",
"sunlab-14",
"sunlab-15",
"sunlab-16",
"sunlab-17",
"sunlab-18",
"sunlab-19",
"sunlab-20",
"sunlab-21",
"sunlab-22"
);


foreach $host (@hosts) {
  system "$ssh $host \"loadserver 1000000 server:tcp:9999 connect:tcp:9998 targt:udp:239.239.239.239:9998\" &";
#  system "$ssh $host \"cd RPS-development/Scripts; perl start_hostloadmeasure.pl; perl start_hostloadpred.pl\"";
}

