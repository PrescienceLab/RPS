#!/usr/bin/env perl

$#ARGV==3 or die "usage: import_sched_results.pl file database table \"description\"";

$file = $ARGV[0];
$database = $ARGV[1];
$table = $ARGV[2];
$desc =$ARGV[3];

system "mysql --user=root --execute=\"insert into descriptions values ('$table','$desc')\" $database";

system "mysql --user=root --execute=\"create table $table (timestamp double not null, hostname varchar(80) not null, predname varchar(20) not null, tnom double not null, slack double not null, numpossible int not null, tlb double not null, texp double not null, tub double not null, tact double not null, inrange varchar(10) not null, deadlinemet varchar(10) not null)\" $database";

system "mysql --user=root --execute=\"load data infile '$file' into table $table ignore 1 lines\" $database";
