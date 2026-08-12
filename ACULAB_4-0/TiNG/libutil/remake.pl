#!/usr/local/bin/perl
$TiNGTARGET = $ENV{'TiNGTARGET'};
die "TiNGTARGET not set\n" unless defined($TiNGTARGET);
$dir = "errs/gen-$TiNGTARGET";
opendir(D, $dir)
	|| die "Cannot open directory '$dir': $!\n";
while ($_ = readdir(D)) {
	next if /^\./;
	my $file = "$dir/$_";
	open(F, $file) || die "Cannot open file: '$file': $!\n";
	while (<F>) {
		next unless / '([^ ]+)': No such file or directory/
			||  / ([^ ]+): No such file or directory/
			||  / cannot find include file: "([^ ]+)"/;
		$needed{$1} = 1;
	}
}
print join(' ', sort(keys(%needed)));
