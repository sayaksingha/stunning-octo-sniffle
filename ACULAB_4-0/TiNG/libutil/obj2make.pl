die "Usage: obj2mak\n" unless @ARGV == 0;
$TiNGTARGET = $ENV{'TiNGTARGET'};
$g_obj = $TiNGTARGET =~/^WINNT/;
$g_prog = '';
$g_prog_cpp = '';
$g_shared = '';
$g_shared_cpp = '';
$g_extra = '';
&obj2mak('objlist', '');
&obj2mak("$TiNGTARGET/objlist", "_THIS");
$makfile = "gen-$TiNGTARGET/objs.mak";
open(MF, ">$makfile") || die "Cannot create file: $makfile: $!\n";
print MF $g_prog, "\n", $g_prog_cpp, "\n";
print MF $g_shared, "\n", $g_shared_cpp, "\n";
print MF $g_extra, "\n";
print MF "\n";

sub obj2mak {
	my ($objfile, $sfx) = @_;
	open(OBJLIST, $objfile) || die "Cannot open file: $objfile: $!\n";
	$g_prog .= "PROG${sfx}_OBJS =";
	$g_prog_cpp .= "PROG_CPP${sfx}_OBJS =";
	$g_shared .= "SHARED${sfx}_OBJS =";
	$g_shared_cpp .= "SHARED_CPP${sfx}_OBJS =";
	$g_extra .= "EXTRA${sfx}_LIBS =";
	while (<OBJLIST>) {
		next if /^\s*#/;
		chop;
		my ($where, $name) = split;
		my $prog;
		my $shared;
		if ($name =~ s/\.opp$/.o/) {
			$prog = \$g_prog_cpp;
			$shared = \$g_shared_cpp;
		} else {
			$prog = \$g_prog;
			$shared = \$g_shared;
		}
		$name =~ s/\.o/.obj/ if $g_obj;
		if ($where eq '.') {
			$$prog .= " \\\n\tgen-$TiNGTARGET/$name";
		} elsif ($where eq 'apilib') {
			$$shared .= " \\\n\t" . '$(TiNG_LIB)';
			$g_extra .= " \\\n\t" . '${TiNG_EXTRA_LIBS}';
		} elsif ($where eq 'switch') {
			$$shared .= " \\\n\t" . '$(SWITCH_LIB)';
			$g_extra .= " \\\n\t" . '${SWITCH_EXTRA_LIBS}';
		} elsif ($where eq 'call') {
			$$shared .= " \\\n\t" . '$(CALL_LIB)';
			$g_extra .= " \\\n\t" . '${CALL_EXTRA_LIBS}';
		} elsif ($where eq 'Testlib') {
			$$shared .= " \\\n\t../Testlib/gen-$TiNGTARGET/$name";
		} else {
			$$shared .= " \\\n\t../../$where/gen-$TiNGTARGET/$name";
		}
	}
	$g_prog .= "\n";
	$g_prog_cpp .= "\n";
	$g_shared .= "\n";
	$g_shared_cpp .= "\n";
	$g_extra .= "\n";
}
