#!/usr/bin/perl
# mkmap - generate a linker control file containing only the
# correct entry points
while (<>) {
	chop;
	my ($entry, $sig) = split;
	print "$entry\n";
}
