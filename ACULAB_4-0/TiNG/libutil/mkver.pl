if ($ARGV[0] eq '-v') {
	die "Usage: mkver -v version-file [-r relver-file]\n" unless @ARGV;
	shift(@ARGV);
	$verfile = shift(@ARGV);
	open(V, $verfile) || die "Cannot open file: $verfile: $!\n";
	$version = <V>;
	close(V);
	chomp($version);
	if ($ARGV[0] eq '-r') {
		die "Usage: mkver -v version-file [-r relver-file]\n" unless @ARGV;
		shift(@ARGV);
		$relverfile = shift(@ARGV);
		open(V, $relverfile) || die "Cannot open file: $relverfile: $!\n";
		$relver = <V>;
		close(V);
		chomp($relver);
	}
	$usage = ' [--version]';
} else {
	die "Usage: mkver -v version-file [-r relver-file]\n";
}
print "\"Aculab Telephony Software V$relver.\" " if defined($relver);
print "\"$version\"\n";

