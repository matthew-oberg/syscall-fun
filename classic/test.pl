#!/usr/local/bin/perl

use strict;
use warnings;

my @result = split(' ', `doas dtrace -s trace.d -c ./streamer.o`);

my $sum;

foreach (@result)
{
    print($_, "\n");
    $sum += $_;
}

print('Average: ', $sum / @result, "\n");

