#!/usr/bin/perl
open(F, "../relver") || die "Cannot open file: ../relver: $!\n";
$_ = <F>;
close(f);
chomp;
if (s/^([0-9]+)\.([0-9]+)\.([0-9]+)//) {
	($ver_maj, $ver_mid, $ver_min) = ($1, $2, $3);
	$ver_code = 'i';
	$ver_level = 0;
	if (s/^([a-z])([0-9]+)//) {
		($ver_code, $ver_level) = ($1, $2);
	}
	if ($_ ne '') {
		print STDERR "Junk after version: '$_'\n";
		die "Cannot read version from file ../relver\n";
	}
} else {
	print STDERR "No version in file: '$_'\n";
	die "Cannot read version from file ../relver\n";
}
open(F, ">gen/relver.i") || die "Cannot create file: gen/relver.i: $!\n";
print F "$ver_maj, $ver_mid, $ver_min, '$ver_code', $ver_level\n";
close(F);
