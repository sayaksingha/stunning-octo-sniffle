#!/usr/local/bin/perl
# expincl - expand include files
$onlyi = 0;
$needall = 0;
@incpath = ();
for (;;) {
	if ($ARGV[0] eq '-i') {
		$onlyi = 1;
		shift(@ARGV);
	} elsif ($ARGV[0] eq '-n') {
		$needall = 1;
		shift(@ARGV);
	} elsif ($ARGV[0] eq '-I') {
		shift(@ARGV);
		push(@incpath, shift(ARGV));
	} elsif ($ARGV[0] =~ s/^-I//) {
		push(@incpath, shift(ARGV));
	} else {
		last;
	}
}
@ARGV == 3 || die "Usage: expinc [-i] [-n] [-I include-dir]* verfile srcfile dstfile\n";
my ($ver, $src, $dst) = @ARGV;
open(VER, $ver) || die "Cannot open version file: $ver: $!\n";
$v = <VER>;
chomp($v);
close(VER);
open(DST, ">$dst") || die "Cannot create file: $dst: $!\n";
$dep = $dst;
$dep =~ s!.*/!!;
$dep = "gen/$dep.M";
open(DEP, ">$dep") || die "Cannot create file: $dep: $!\n";
print DEP "$dst:";
print DEP " \\\n\t$ver";
print DST "// Automatically generated (from $v) - DO NOT EDIT!\n";
for (@incpath) {
	s!/*$!/! if $_ ne '';
}
$retval = 0;
if (&expinc('', $src)) {
	print STDERR "Included in $src\n";
	$retval = 1;
}
print DEP "\n";
exit($retval);

sub expinc {
	my ($dir, $file) = @_;
	local($_);
	local(@path) = @incpath;
	if ($file =~ s!(.*/)!!) {
		if ($dir eq '') {
			$dir = $1;
		} else {
			$dir .= "/$1";
		}
	}
	for (;;) {
		last if -e "$dir$file";
		if (!@path) {
			if ($needall) {
				print STDERR "Cannot find file: $file\n";
				return 1;
			}
			print "Omitting missing file: $file\n";
			print DST <<EOF;
/* Since this file has been pre-processed to put #includes inline,
 * and the pre-processor doing this found a #include here which referred
 * to a missing file, it assumed that this is within a #if/#ifdef and the
 * #include will be ignored. An #error is used here just in case someone
 * uses this file with the options which require this section.
 */
EOF
			print DST "#error include file built without $file\n";
			return 0;
		}
		$dir = shift(@path);
	}
	$file = "$dir$file";
	$dir = $file;
	if ($dir =~ /\//) {
		$dir =~ s!/+[^/]*$!!;
		$dir .= '/' unless $dir eq '';
	} else {
		$dir = '';
	}
	local(*FILE);
	open(FILE, "$file") || die "Cannot open file: $file: $!\n";
	print DEP " \\\n\t$file";
	while (<FILE>) {
		if (/^\s*#\s*include\s*"([^"]*)"\s*$/) {
			my $incfile = $1;
			if (!$onlyi || $incfile =~ /\.[ic]$/) {
				print DST "// start $_";
				if (&expinc($dir, $incfile)) {
					print STDERR "Included in $file\n";
					return 1;
				}
				$_ = "// end " . $_;
			}
			else
			{
				$_ = "#include \"../apilib/".$dir.$incfile."\"\n";
			}
		}
		print DST $_;
	}
	close(FILE);
	return 0;
}
