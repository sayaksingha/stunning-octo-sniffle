#!/usr/local/bin/perl
# fix a gcc dependency file. Prior to version 3.0, when the -MD
# option is used on gcc, it puts the output in the wrong place (it
# derives the file name from the source instead of the target)
# and the contents are also wrong because the target lacks its
# directory. i.e. gcc -o d/bar.o -c foo.c creates foo.d instead of
# d/bar.d and that file contains "bar.o: foo.c" instead of
# "d/bar.o: foo.c"
#
# This script fixes this (and uses *.M as the fixed name)

@ARGV == 2 || @ARGV == 1 || die "Usage: fixdep target [source]\n";
$_ = shift;
/^(.*\/)([^\/]*\.)t?o$/ || die "Expected name to be */*.o: $_\n";
$targetdir = $1;
$targetfilebase = $2;
if (@ARGV) {
	$_ = shift;
	/^([^\/]*\.)c$/
		|| /^([^\/]*\.)cpp$/
		|| die "Expected name to be *.c or *.cpp: $_\n";
	$sourcefilebase = $1;
} else {
	$sourcefilebase = $targetfilebase;
}
$dfile = $sourcefilebase . "d";
$mfile = $targetdir . $targetfilebase . "M";
$okdfile = $targetdir . $targetfilebase . "d";
$hasok = -f $okdfile;
$hasbad = -f $dfile;
if ($hasok && $hasbad) {
	my $okt = -M $okdfile;
	my $badt = -M $dfile;
	if ($okt > $badt) {
		$hasok = 0;
		print STDERR "Both $dfile and $okdfile exist. Using $dfile\n";
		unlink($okdfile) || die "Cannot unlink $okdfile: $!\n";
	} else {
		$hasbad = 0;
		print STDERR "Both $dfile and $okdfile exist. Using $okdfile\n";
		unlink($dfile) || die "Cannot unlink $dfile: $!\n";
	}
}
if ($hasok) {	# version 3.0 or later
	rename($okdfile, $mfile)
		|| die "Cannot rename $okdfile to $mfile: $!\n";
} elsif ($hasbad) {
	open(DFILE, $dfile) || die "Cannot open $dfile: $!\n";
	open(MFILE, ">$mfile") || die "Cannot create $mfile: $!\n";
	print MFILE $targetdir;
	while (<DFILE>) {
		print MFILE $_;
	}
	close(MFILE) || die "Error closing file: $mfile: $!\n";
	close(DFILE);
	unlink($dfile) || die "Cannot delete $dfile: $!\n";
} else {
	die "Neither $dfile nor $okdfile exist.\n";
}
