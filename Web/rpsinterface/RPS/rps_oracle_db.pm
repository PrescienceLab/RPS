#!/usr/local/bin/perl -w

package rps_oracle_db;

use strict;
use URI::Escape;
use DBI;


sub new
 {
  my $self = {};
  $self->{"user"} = undef;
  $self->{"passwd"} = undef;
  $self->{"dbpath"} = undef;
  bless($self);
  return $self;
 }

sub RPSDBinit
 {
  my $self = shift;
  my($user,$passwd,$path) = @_;

  $self->{"user"} = $user;
  $self->{"passwd"} = $passwd;
  $self->{"dbpath"} = $path;

  return $self;
 }

sub RPSDBadd
 {
  my $self = shift;
  my ($resourcetype,
      $client,
      $protocol,
      $address,
      $port) = @_;
  my $path = $self->{"dbpath"};
  my $dbh = DBI->connect("DBI:Oracle:", $self->{"user"}, $self->{"passwd"});
  my $prep = $dbh->prepare("insert INTO $path \(sensortype, client, protocol, address, port) 
			    VALUES \(\'$resourcetype\',\'$client\',\'$protocol\',\'$address\',\'$port\')");
  $prep->execute();
  $prep->finish();
  $dbh->disconnect();
 }

sub RPSDBdelete
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my ($deleteresource) = @_;
  my @deleteresource = split(' ',$deleteresource);
  my $dbh = DBI->connect("DBI:Oracle:",$self->{"user"}, $self->{"passwd"});
  $dbh->do("DELETE 
            FROM $path c 
            WHERE c.address = \'$deleteresource[2]\' AND
                  c.client = \'$deleteresource[3]\' AND
                  c.sensortype = \'$deleteresource[0] $deleteresource[1]\'");
  $dbh->disconnect(); 
 }

sub RPSDBresources
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my @sensortype;
  my @address;
  my @client;
  my @resources;
  my $dbh = DBI->connect("DBI:Oracle:", $self->{"user"}, $self->{"passwd"});
  my $prep = $dbh->prepare("SELECT UNIQUE c.sensortype, c.address, c.client
		                FROM $path c");
  $prep->execute();
  my ($sensortype,$address,$client);
  $prep->bind_columns(undef, \$sensortype,\$address,\$client);

  while ($prep->fetch())
   {
    @sensortype = split(' ',$sensortype);
    @address = split(' ',$address);
    @client  = split(' ',$client);
    push(@resources,"$sensortype[0] $sensortype[1] $address[0] $client[0]");
   }
  $prep->finish();
  $dbh->disconnect();
  return @resources;
 }


sub RPSDBbuffersyscalls
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my ($numgraphs, 
      $measurebufferclientnum, 
      $resource,
      $resourcetype) = @_;
  my @syscalls;
  
  my @selectedresource = split(' ',$resource);
  my $address = pop(@selectedresource);
  
  my $dbh = DBI->connect("DBI:Oracle:", $self->{"user"}, $self->{"passwd"});
  my $prep = $dbh->prepare("SELECT UNIQUE c.client, c.protocol, c.port
                            FROM $path c
                            WHERE c.address = \'$address\' AND c.sensortype = \'$resourcetype\'");
  $prep->execute();
  my ($client,$protocol, $port);
  $prep->bind_columns(undef, \$client, \$protocol, \$port);
  
  while ($prep->fetch())
   {
    my @client = split(' ',$client);
    my @protocol = split(' ',$protocol);
    my @port = split(' ',$port);
     
    if ($client[0] eq "predbufferclient")
     {
      push(@syscalls, "$client[0] $numgraphs client:$protocol[0]:$address:$port[0]");
     }
    elsif ($client[0] eq "measurebufferclient")
     {
      push(@syscalls, "$client[0] $measurebufferclientnum client:$protocol[0]:$address:$port[0]");
     }
   }
  $prep->finish();
  $dbh->disconnect();
  
  return @syscalls;
 }


sub RPSDBstreamsyscalls
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my ($resource,$resourcetype) = @_;
  my @syscalls;
  
  my @selectedresource = split(' ',$resource);
  my $address = pop(@selectedresource);
  
  my $dbh = DBI->connect("DBI:Oracle:", $self->{"user"}, $self->{"passwd"});
  my $prep = $dbh->prepare("SELECT UNIQUE c.client, c.protocol, c.port
                            FROM $path c
                            WHERE c.address = \'$address\' AND c.sensortype = \'$resourcetype\'");
  $prep->execute();
  my ($client,$protocol, $port);
  $prep->bind_columns(undef, \$client, \$protocol, \$port);
  
  while ($prep->fetch())
   {
    my @client = split(' ',$client);
    my @protocol = split(' ',$protocol);
    my @port = split(' ',$port);
     
    if ($client[0] eq "predclient")
     {
      push(@syscalls, "predclient source:$protocol[0]:$address:$port[0]");
     }
    elsif ($client[0] eq "measureclient")
     {
      push(@syscalls, "measureclient source:$protocol[0]:$address:$port[0]");
     }
   }
  $prep->finish();
  $dbh->disconnect();
  
  return @syscalls;
 }

sub RPSDBaddresses
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my ($resourcetype) = @_;
  my @resources;

  my $dbh = DBI->connect("DBI:Oracle:", $self->{"user"}, $self->{"passwd"});
  my $prep = $dbh->prepare("SELECT UNIQUE c.address
		                FROM $path c
                            WHERE c.sensortype = \'$resourcetype\'");
  $prep->execute();
  my ($address);
  $prep->bind_columns(undef,\$address);

  while ($prep->fetch())
   {
    my @address = split(' ',$address);
    push(@resources,"$address[0]");
   }
  $prep->finish();
  $dbh->disconnect();
  return @resources;
 }


sub RPSDBshut
 {
  my $self = shift;
  $self->{"user"} = "";
  $self->{"passwd"} = "";
  $self->{"path"} = "";
 }

 
1;
