# checkmod.pl - check modification times of a group of files
#
# when using version control, it is necessary to update the version
# code when the first modification is made to any file (so that the
# version indicates that the result is not a valid version). This
# can be done by 'make', but only by checking for a modification on
# every build. This script provides the functionality needed to check
# only if no modification has yet been detected.
#
# To use it, specify in the Makefile:
#	gen/project-mod: project-v
#		perl checkmod.pl project
#
#	somefile: gen/project-mod
#		command to make somefile from version code gen/project-ver
#
# and use 'gen/project-ver' as the version code. This works by
# re-running the script every time until a modification is detected.
# 'make' re-runs the script because the file 'gen/project-mod' is not
# created. When a modification is detected, that file is created, so
# for future runs it will be newer than 'project-v' so the script is
# never run again.
#
@ARGV == 1 || die "Usage: checkmod groupname\n";
$group = shift;
-d 'gen' || mkdir('gen', 0777) || die "Cannot create directory: gen: $!\n";
$vfile = $group . "-v";
$flist = $group . "-l";
$modfile = "gen/$group-mod";
$verfile = "gen/$group-ver";
open(VFILE, $vfile) || die "Cannot open version file: $vfile: $!\n";
$version = <VFILE>;
close(VFILE) || die "Error closing file: $vfile: $!\n";
open(MODDBG, ">$modfile.dbg") || die "Cannot create file: $modfile.dbg: $!\n";
chomp($version);
$vfile_mod = -M $vfile;
if (&already_mod($modfile, $vfile_mod)) {
	print MODDBG "existing file ($modfile) is up to date\n";
	$haschange = 1;
} else {
	$haschange = 0;
	if (-f $flist) {
		print MODDBG "Checking files on list $flist\n";
		if (&checksimple($flist, $vfile_mod)) {
			$haschange = 1;
		}
	} else {
		print MODDBG "Checking files on MD5 lists\n";
		my @md5 = <*.md5>;
		@md5 || die "Cannot find *.md5 - no list of files to check\n";
		for $flist (@md5) {
			if (&checkmd5($flist, $vfile_mod)) {
				$haschange = 1;
				last;
			}
		}
	}
	if ($haschange) {
		$version =~ s/\.$/+/;
		open(MODFILE, ">$modfile")
			|| die "Cannot create mod file: $modfile: $!\n";
		close(MODFILE) || die "Error closing file: $modfile: $!\n";
	}
}
if ($haschange) {
	$version =~ s/\.$/+/;
}
&mkver($verfile, $version);

# This doesn't actually check the MD5 checksum as that would be too slow
# and it's unnecesssary as a local check since the file modification
# time is sufficient.
sub checksimple {
	my ($flist, $vfile_mod) = @_;
	open(FLIST, $flist) || die "Cannot open file list: $flist: $!\n";
	while (<FLIST>) {
		chop;
		next if /^gen[-\/]/ || /\/gen[-\/]/;
		my $mt = -M $_;
		if ($mt < $vfile_mod) {
			print "Newer: $_\n";
			print MODDBG "Newer: $_: $mt (", &mtime($_), ") < $vfile_mod (", &mtime($vfile), ")\n";
			return 1;
		}
	}
	return 0;
}

sub checkmd5 {
	my ($flist, $vfile_mod) = @_;
	local($_);
	local(*FLIST);
	open(FLIST, $flist) || die "Cannot open file list: $flist: $!\n";
	while (<FLIST>) {
		chop;
		if (/^([^ ]+)  (.*)/) {
			my ($sum, $file) = ($1, $2);
			if (-f $file) {
				my $cmd = "md5sum $file";
				local(*F);
#				print "# $cmd\n";
				open(F, "$cmd|") || die "Cannot run '$cmd': $!\n";
				if (<F> =~ /^([^ ]*) [ \*](.*)/) {
					my ($rs, $rf) = ($1, $2);
					die "Command: '$cmd', said file was '$rf', not '$file'\n" if $rf ne $file;
					if ($rs ne $sum) {
						print "MD5 changed: $sum -> $rs: $file\n";
						print MODDBG "MD5 changed: $sum -> $rs: $file\n";
						return 1;
					}
				} else {
					die "No result from command: '$cmd'\n";
				}
			} else {
				print "File do not exist: $file\n";
				print MODDBG "File does not exist: $file\n";
				return 1;
			}
		}
	}
	return 0;
}

sub mkver {
	my ($verfile, $version) = @_;
	local(*VERFILE);
	if (-f $verfile) {
		open(VERFILE, "$verfile") || die "Cannot read version file: $verfile: $!\n";
		my $oldver = <VERFILE>;
		chomp($oldver);
		close(VERFILE) || die "Error closing file: $verfile: $!\n";
		if ($oldver eq $version) {
			print "Version already ok\n";
			return;
		}
	}
	open(VERFILE, ">$verfile") || die "Cannot create version file: $verfile: $!\n";
	binmode VERFILE;
	print VERFILE $version, "\n";
	close(VERFILE) || die "Error closing file: $verfile: $!\n";
}

sub already_mod {
	my ($modfile, $vfile_mod) = @_;
	return 0 unless -f $modfile;
	return 1 if -M $modfile <= $vfile_mod;
	unlink($modfile) || die "Cannot remove stale mod file: $modfile: $!\n";
	return 0;
}

sub mtime {
	my ($f) = @_;
	my @s = stat($f);
	@s || die "Cannot stat $f: $!\n";
	my @t = gmtime($s[9]);
	return sprintf("%04d-%02d-%02d %02d:%02d:%02d",
		$t[5] + 1900, $t[4] + 1, $t[3], $t[2], $t[1], $t[0]);
}
