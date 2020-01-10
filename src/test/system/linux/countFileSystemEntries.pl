#!/usr/bin/perl

use strict;
use warnings;

my $fstype=$ARGV[0];
my $key;
my $count=0;
my $out;
my @out;
my %list;
keys(%list) = `cat /etc/mtab | grep -c $fstype`+`cat /etc/fstab | grep -c $fstype`;


# get list of mounted file systems

@out=`cat /etc/mtab | grep $fstype`;
foreach $out (@out) {
  my ($dev,$root,@val) = split(' ',$out);
  $list{$root}=$dev;
}

# get list of in /etc/fstab registered file systems

@out=`cat /etc/fstab | grep $fstype`;
foreach $out (@out) {
  my ($dev,$root,@val) = split(' ',$out);
  my $found = 0;
  foreach $key (keys %list) {
    $found = 1 if($key eq $root);
  }
  $list{$root}=$dev if($found==0);
}

# count entries
foreach $key (keys %list) {
  $count++;  
}

print "$count\n";
exit;
