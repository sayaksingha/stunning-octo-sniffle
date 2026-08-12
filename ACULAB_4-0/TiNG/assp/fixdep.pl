#!/usr/local/bin/perl
# fix a gcc dependency file. Prior to version 3.0, when the -MD
# option is used on gcc, it puts the output in the wrong place (in
# the same directory as the source file instead of as the target
# file) and the contents are also wrong because the target lacks its
# directory. i.e. gcc -o d/foo.o -c foo.c creates foo.d instead of
# d/foo.d and that file contains "foo.o: foo.c" instead of
# "d/foo.o: foo.c"
#
# This script fixes this (and uses *.M as the fixed name)
@ARGV == 1 || @ARGV == 2|| die "Usage: fixdep target [source]\n";
$_ = shift;
/^(.*\/)([^\/]*\.)t?o$/ || die "Expected name to be .*/.*o: $_\n";
$targetdir = $1;
$targetfilebase = $2;
if (@ARGV) {
	$_ = shift;
	/^(.*\.)c$/ || die "Expected name to be *.c: $_\n";
	$sourcefilebase = $1;
	$dfile = $sourcefilebase . "d";
} else {
	$dfile = $targetfilebase . "d";
}
$mfile = $targetdir . $targetfilebase . "M";
$okdfile = $targetdir . $targetfilebase . "d";
$hasok = -f $okdfile;
$hasbad = -f $dfile;
if ($hasok) {	# version 3.0 or later
	if ($hasbad) {
		die "Both $dfile and $okdfile exist. Don't know which to believe!\n";
	}
	rename($okdfile, $mfile)
		|| die "Cannot rename $okdfile to $mfile: $!\n";
} elsif ($hasbad) {
	open(DFILE, $dfile) || die "Cannot open $dfile: $!\n";
	open(MFILE, ">$mfile") || die "Cannot create $mfile: $!\n";
	print MFILE $targetdir;
	while (<DFILE>) {
		print MFILE $_;
	}
	close(MFILE);
	close(DFILE);
	unlink($dfile) || die "Cannot delete $dfile: $!\n";
} else {
	die "Neither $dfile nor $okdfile exist.\n";
}
