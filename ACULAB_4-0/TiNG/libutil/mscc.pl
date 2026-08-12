#!/usr/bin/perl
# this finds all dependencies and runs the compiler
# it is intended as a substitute for GCC's -MD option when using
# the Microsoft C compiler. Just pass in the arguments you would
# have passed to CL.
#
# the include files are found in accordance with the rules in
# the Visual C help documentation. It is always possible that either a
# change to the compiler or an inaccuracy in the documentation would
# make this script use a different file to the compiler - that's why
# this should really be a feature of the compiler.
#
$| = 1;
$compiler = $ENV{'MS_COMPILER'};
$compiler = 'cl' unless defined($compiler);
@srcfiles = ();
%donefile = ();
@incpath = ();
$g_filehandle = "INFILE_0";
for (;;) {
	if ($ARGV[0] eq '-outfile') {
		shift;
		$objfile = shift;
	} elsif ($ARGV[0] eq '-nesting') {
		shift;
		$nesting = 1;
		my $nf = shift(@ARGV);
		open(NEST, ">$nf") || die "Cannot create file: $nf: $!\n";
	} elsif ($ARGV[0] eq '-verbose') {
		$verbose = 1;
		shift;
	} else {
		last;
	}
}
@args = @ARGV;
while ($_ = shift) {
	if (s/^[-\/]//) {	# an option
		if (s/^Fo// || s/^Fe//) {
			die "Object file defined twice: $objfile and $_\n" if defined($objfile);
			if ($_ eq '') {
				$objfile = shift;
			} else {
				$objfile = $_;
			}
		} elsif (s/^I//) {
			if ($_ eq '') {
				push(@incpath, shift);
			} else {
				push(@incpath, $_);
			}
		} elsif (s/^D//) {
			shift if $_ eq '';	# if next arg is definition
		}
	} else {
		push(@srcfiles, $_);
	}
}
@incpath = (@incpath, split(/;/, $ENV{'INCLUDE'}));
die "No object file specified\n" unless defined($objfile);
die "No source file specified\n" unless @srcfiles;
for $file (@srcfiles) {
	my $fh = $g_filehandle++;
	open($fh, "$file") || die "Cannot open file: $file: $!\n";
	&includes($file, $fh, 0);
	close($fh);
}
open(DEPS, ">$objfile.M") || die "Cannot create file $objfile.M: $!\n";
print DEPS "$objfile: \\\n\t";
print DEPS join(" \\\n\t",
	map(&quotefile($_),
		sort(
			grep(&okpath($_), keys(%donefile))
	))), "\n";
close(DEPS);
print STDERR "exec cl ", join(" ", @args), "\n" if $verbose;
system("$compiler " . join(' ', @args)); 
#exec $compiler, @args;		- not sure why this fails on some systems
#print STDERR "exec cl failed: $!\n";

sub includes {
	my ($file, $fh, $level) = @_;
	$file = &canonfile($file);
	return if defined($donefile{$file});
	# the following line prints the inclusion tree at the start
	print NEST " " x $level, $file, "\n" if $nesting;
	if ($file =~ /(.*)\/[^\/]*$/) {
		unshift(@incpath, $1);
	} else {
		unshift(@incpath, ".");
	}
	$donefile{$file} = 1;
	while (<$fh>) {
			# should really do <...> also, but it
			# leads to very many files being checked
			# which makes 'make' very slow
		next unless # /^[ \t]*#include[ \t]*<(.*)>/ ||
			/^[ \t]*#include[ \t]*"(.*)"/;
		my $incfile = $1;
		my $dir;
		for $dir (@incpath) {
			my $inc = "$dir/$incfile";
			my $ifh = $g_filehandle++;
			if (open($ifh, $inc)) {
				&okpath($dir) && &includes($inc, $ifh, $level+1);
				close($ifh);
				goto foundinc;
			} elsif ($! ne 'No such file or directory') {
				die "Cannot open file: $inc: $!\n";
			}
		}
		#print STDERR "Cannot find include file: $incfile\n";
		foundinc:
		# @incpath
		# directories from environment variable INCLUDE
	}
	shift(@incpath);
}

sub canonfile {
	my $f = $_[0];
	$f =~ s!\\!/!g;	# '\' -> '/'
	my $isunc = 0;
	$isunc = 1 if $f =~ s@^//@/@;
	my @o = ();
	my $d;
	for $d (split(/\//, $f)) {
		if ($d eq '' || $d eq '.') {
		} elsif ($d eq '..') {
			if (@o && $o[$#o] ne '..') {
				pop(@o);
			} else {
				push(@o, $d);
			}
		} else {
			push(@o, $d);
		}
	}
	$d = join('/', @o);
	$d = "/" . $d if $f =~ /^\//;
	$d = '.' if $d eq '';
	$d = "/$d" if $isunc;
	return $d;
}

sub quotefile {
	my $f = $_[0];
	my $o = '';
	while ($f ne '') {
		if ($f =~ s/^([ \$\\])//) {
			$o .= '\\' . $1;
		} elsif ($f =~ s/^([^ \$\\]+)//) {
			$o .= $1;
		}
	}
	return $o;
}

	# check for system directories which are not worth handling
	# This is an alternative to handling <...> and "..." differently
sub okpath {
	return 1;	# not needed when <...> is ignored
	my $dir = $_[0];
	return 0 if $dir =~ /e:\/msdn\//;
	return 0 if $dir =~ /F:\/NT2000DDK\//;
	return 0 if $dir =~ /F:\/DDK-NT\//;
	return 0 if $dir =~ /c:\/mstools\//;
	return 1;
}
