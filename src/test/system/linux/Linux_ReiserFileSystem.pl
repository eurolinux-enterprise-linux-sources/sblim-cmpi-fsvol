#!/usr/bin/perl

use strict;
use warnings;

my $className="Linux_ReiserFileSystem.instance";
my $INST_FILE;

my $IN=$ARGV[0];

if(defined $IN) {
  if($IN =~ /-rm/) {
    unlink("$className");
    exit 0;
  }
}

my %list;
keys(%list) = `cat /etc/mtab | grep -c reiserfs`+`cat /etc/fstab | grep -c reiserfs`;
my @out;
my $out;


# get list of mounted file systems

@out=`cat /etc/mtab | grep reiserfs`;
foreach $out (@out) {
  my ($dev,$root,@val) = split(' ',$out);
  $list{$root}=$dev;
}

# get list of in /etc/fstab registered file systems
my $key;
@out=`cat /etc/fstab | grep reiserfs`;
foreach $out (@out) {
  my ($dev,$root,@val) = split(' ',$out);
  my $found = 0;
  foreach $key (keys %list) {
    $found = 1 if($key eq $root);
  }
  $list{$root}=$dev if($found==0);
}


# create .instance file

if( !open($INST_FILE,'>', "$className")) {
  print "can not open $className\n"; 
  exit 0;
}

foreach my $key (keys %list) {
  my $state = `mount | grep -c ' $key '`;
  if($state==1) { $state=2; }
  elsif($state==0) { $state=3; }

  print $INST_FILE "$list{$key}\n";
  print $INST_FILE "$state\n";
  print $INST_FILE "$state\n";
  print $INST_FILE "$key\n";
  print $INST_FILE "$key\n";
  print $INST_FILE "\n";
}

