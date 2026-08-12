#!/usr/bin/perl
while (<>) {
	next if /^\s/;
	chomp;
	my ($num, $name) = split;
	die "Error name is too long: $name\n" unless 80 > length $name; 
	print "\tcase $name: name = \"$name\"; break;\n";
}
