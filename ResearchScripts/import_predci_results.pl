#!/usr/local/bin/perl -w

$#ARGV==3 or die "usage: import_predci_results.pl file database table \"description\"";

$file = $ARGV[0];
$database = $ARGV[1];
$table = $ARGV[2];
$desc =$ARGV[3];

system "mysql --user=root --execute=\"insert into descriptions values ('$table','$desc')\" $database";

system "mysql --user=root --execute=\"create table $table (timestamp double not null, model varchar(80) not null, tnom double not null, tlb double not null, texp double not null, tub double not null, tact double not null, usrtime double not null, systime double not null, tau double not null, meanload double not null, discountedmeanload double not null, varload double not null)\" $database";

system "mysql --user=root --execute=\"load data infile '$file' into table $table ignore 1 lines\" $database";
