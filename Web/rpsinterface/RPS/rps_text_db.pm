#!/usr/bin/env perl 

package rps_text_db;

use strict;
use URI::Escape;



sub new
 {
  my $self = {};
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
  my $dbcontents = "";
  my $path = $self->{"dbpath"};
  open(INFILE, "<$path");
  while(<INFILE>)
    {
     $dbcontents .=$_;
    }
  open(OUTFILE, ">$path");
  print OUTFILE $dbcontents . "$resourcetype,$client,$protocol,$address,$port\n";
  close(OUTFILE);
 }

sub RPSDBdelete
 {
  my $self = shift;
  my ($deleteresource) = @_;
  my $path = $self->{"dbpath"};
  my @deleteresource = split(' ',$deleteresource);
  my @contents;
  open(INFILE, "<$path");
  while (<INFILE>)
   {
    push(@contents, $_);
   }
  close(INFILE);
  open(OUTFILE, ">$path");
  foreach(@contents)
   {
    my @line = split(',',$_);
    if ($line[0] ne "$deleteresource[0] $deleteresource[1]" || 
        $line[3] ne $deleteresource[2] || 
        $line[1] ne $deleteresource[3])
     {
      print OUTFILE "$line[0],$line[1],$line[2],$line[3],$line[4]";
     }
   }
  close(OUTFILE);    
 }

sub RPSDBresources
 {
  my $self = shift;
  my $path = $self->{"dbpath"};
  my @resources;
  my @sensortype;
  my @address;
  my @client;

  open(INFILE,"<$path");
  while(<INFILE>)
   {
    my @line = split(',',$_);
    push(@resources,"$line[0] $line[3] $line[1]");
   }
  close(INFILE);
  return @resources;
 }

sub RPSDBbuffersyscalls
 {
  my $self = shift;
  my ($numgraphs, $measurebufferclientnum, $resource) = @_;
  my @syscalls;
  my %selectdbcontents;
  my $path = $self->{"dbpath"};
 
  my @selectedresource = split(' ',$resource);
  my $address = pop(@selectedresource);
 
  open(INFILE,"<$path");
  while(<INFILE>)
   {
    my @line = split(',',$_);
    if ($address eq $line[3])
     {
      $selectdbcontents{"$line[1],$line[2],$line[4]"} = $address;
     }
   }
  close(INFILE);
  foreach(%selectdbcontents)
   {
    my @line = split(',',$_);
    
    if ($line[0] eq "predbufferclient")
     {
      push(@syscalls, "$line[0] $numgraphs client:$line[1]:$address:$line[2]");
     }
    elsif ($line[0] eq "measurebufferclient")
     {
      push(@syscalls, "$line[0] $measurebufferclientnum client:$line[1]:$address:$line[2]");
     }
   }
  
  return @syscalls;
 }


sub RPSDBstreamsyscalls
 {
  my $self = shift;
  my ($resource) = @_;
  my @syscalls;
  my %selectdbcontents;
  my $path = $self->{"dbpath"};
 
  my @selectedresource = split(' ',$resource);
  my $address = pop(@selectedresource);
 
  open(INFILE,"<$path");
  while(<INFILE>)
   {
    my @line = split(',',$_);
    if ($address eq $line[3])
     {
      $selectdbcontents{"$line[1],$line[2],$line[4]"} = $address;
     }
   }
  close(INFILE);
  foreach(%selectdbcontents)
   {
    my @line = split(',',$_);
    
    if ($line[0] eq "predclient")
     {
      push(@syscalls, "predclient source:$line[1]:$address:$line[2]");
     }
    elsif ($line[0] eq "measureclient")
     {
      push(@syscalls, "measureclient source:$line[1]:$address:$line[2]");
     }
   }
  
  return @syscalls;
 }

sub RPSDBaddresses
 {
  my $self = shift;
  my ($resourcetype) = @_;
  my %contents;
  my @keys;
  my @resources;
  my $path = $self->{"dbpath"};
  
  open(INFILE,"<$path");
  while(<INFILE>)
   {
    my @line=split(',',$_);
    if($line[0] eq $resourcetype)
     {
      $contents{$line[3]}=$line[0];
     } 
   }
  close(INFILE);
  @keys = keys%contents;
  foreach(@keys)
   {
    push(@resources,$_);
   }
    return @resources;
 }

sub RPSDBshut
 {
  my $self = shift;
  $self->{"user"} = "";
  $self->{"passwd"} = "";
  $self->{"dbpath"} = "";
 }
 
1;
