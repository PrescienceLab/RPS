#!/usr/local/bin/perl -w


use strict;
use CGI;
use URI::Escape;
use DBI;
use Getopt::Long;



$ENV{"PATH"} .= ":/home/shoykhet/RPS-development/bin/I386/LINUX";

system("pred_reqresp_server server:tcp:7777 &");


